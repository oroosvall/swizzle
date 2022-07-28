# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

import os

import bpy
import struct
from mathutils import Matrix, Vector, Color
from bpy_extras import io_utils, node_shader_utils

from bpy_extras.wm_utils.progress_report import (
    ProgressReport,
    ProgressReportSubstep,
)

from operator import itemgetter

if "bpy" in locals():
    import importlib
    if "get_animations" in locals():
        importlib.reload(get_animations)

from . import get_animations

HEADER_MAGIC = "SWM "
UV_BIT = 0
NORMAL_BIT = 1
COLOR_BIT = 2

ANIM_BIT = 8

def mesh_triangulate(me):
    import bmesh
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me)
    bm.free()


def savable_objects(objs):
    ob = []
    for o in objs:
        if o.type == 'MESH':
            ob.append(o)
    return ob


def select_vertex_groups(group):

    grp = []

    for g in group:
        grp.append([g.group, g.weight])
    
    while (len(grp) < 4):
        grp.append([0, 0.0])
    if len(grp) > 4:
        grp.sort(key=itemgetter(1), reverse=True)
        grp = grp[:4]
    sum = 0
    for g in grp:
        sum += g[1]

    if sum != 0:
        for g in grp:
            g[1] /= sum
    return tuple(tuple(g) for g in grp)


def generate_vertex_data(mesh, inc_uvs=False, inc_color=False, inc_vgroups=False):
    vertex_data = dict()
    triangle_list = []

    me_verts = mesh.vertices
    me_loops = mesh.loops

    v_counter = 0

    for poly in mesh.polygons:
        tri = []
        for l_idx in poly.loop_indices:
            loop = me_loops[l_idx]
            v_idx = loop.vertex_index
            vpos = me_verts[v_idx].co.copy().freeze()
            v_norm = loop.normal.copy().freeze()
            if mesh.uv_layers.active and inc_uvs:
                v_uv = mesh.uv_layers.active.data[l_idx].uv.copy().freeze()
            else:
                v_uv = None
            
            if mesh.vertex_colors.active and inc_color:
                v_color = mesh.vertex_colors.active.data[l_idx].color
            else:
                v_color = None
            
            if inc_vgroups:
                v_group = select_vertex_groups(me_verts[v_idx].groups)
            else:
                v_group = None


            vertex = (vpos, v_norm, v_uv, v_color, v_group)
            if vertex in vertex_data:
                tri.append(vertex_data[vertex])
            else:
                tri.append(v_counter)
                vertex_data[vertex] = v_counter
                v_counter += 1
        assert len(tri) == 3, "expected 3 triangles"
        triangle_list.append(tri)
    
    vertex_data_rev = {value:key for key, value in vertex_data.items()}

    return (vertex_data_rev, triangle_list)


def get_mesh_objects(objects, context, global_matrix, use_mesh_modifiers):
    depsgraph = context.evaluated_depsgraph_get()
    mesh_objs = []

    for ob in objects:

        backup_pose_positions = []

        for mod in ob.modifiers:
            if mod.type == 'ARMATURE':
                object = mod.object
                if object and object.type == 'ARMATURE':
                    armature = object.data
                    backup_pose_positions.append((armature, armature.pose_position))
                    armature.pose_position = 'REST'

        if backup_pose_positions:
            depsgraph.update()

        ob_for_convert = ob.evaluated_get(depsgraph) if use_mesh_modifiers else ob.original

        me = bpy.data.meshes.new_from_object(
                            ob_for_convert, preserve_all_data_layers=True, depsgraph=depsgraph)

        for armature, pose_position in backup_pose_positions:
            armature.pose_position = pose_position

        if backup_pose_positions:
            depsgraph.update()

        mesh_triangulate(me)

        me.transform(global_matrix)
        me.calc_normals_split()

        mesh_objs.append((ob_for_convert, me))

    return mesh_objs


def set_bit(value, bit):
    return value | (1<<bit)


def clear_bit(value, bit):
    return value & ~(1<<bit)


def is_bit_set(value, bit):
    return (value & (1<<bit)) != 0


def select_version(ver, objs):
    if ver == "1.0":
        return (1, 0, True)
    else:
        return (1, 0, True)


def write_header(fw, major, minor):
    fw(bytes(HEADER_MAGIC, "utf-8"))
    fw(struct.pack("HH", major,minor))


def write_geometry(fw, hasAnimations, mesh_objs, global_matrix):

    fw(struct.pack("B", len(mesh_objs)))
    for ob, me in mesh_objs:
        fw(struct.pack("B", len(ob.name)))
        fw(bytes(ob.name, "utf-8"))
        flags = 0

        obj_arm = None
        for mod in ob.modifiers:
            if mod.type == 'ARMATURE' and mod.object != None:
                obj_arm = mod.object

        anim = hasAnimations and obj_arm != None

        vertex_data, triangle_data = generate_vertex_data(me, True, True, anim)

        verts = []
        normals = []
        uvs = []
        colors = []

        vIndex = []
        vWeights = []


        if me.uv_layers.active:
            flags = set_bit(flags, UV_BIT)

        if me.vertex_colors.active:
            flags = set_bit(flags, COLOR_BIT)

        if anim:
            flags = set_bit(flags, ANIM_BIT)

        flags = set_bit(flags, NORMAL_BIT)

        for key in vertex_data:
            p,n,u,c,g = vertex_data[key]
            verts.extend([*p])
            normals.extend([*n])
            if is_bit_set(flags, UV_BIT):
                uvs.extend([*u])
            if is_bit_set(flags, COLOR_BIT):
                cc = list(map(lambda f:int(round(f* 255.0)), [*c]))
                colors.extend(cc)
            if is_bit_set(flags, ANIM_BIT):
                gr = list(zip(*g))
                vIndex.extend([*gr[0]])
                vWeights.extend([*gr[1]])
                pass

        triangles = []
        for t in triangle_data:
            triangles.extend([*t])

        fw(struct.pack("H", flags))

        fw(struct.pack("I", len(verts)//3))
        fw(struct.pack('%sf' % len(verts), *verts))
        if is_bit_set(flags, UV_BIT):
            fw(struct.pack('%sf' % len(uvs), *uvs))
        if is_bit_set(flags, NORMAL_BIT):
            fw(struct.pack('%sf' % len(normals), *normals))
        if is_bit_set(flags, COLOR_BIT):
            fw(struct.pack('%sB' % len(colors), *colors))
        if is_bit_set(flags, ANIM_BIT):
            fw(struct.pack('%sH' % len(vIndex), *vIndex))
            fw(struct.pack('%sf' % len(vWeights), *vWeights))

        fw(struct.pack("I", len(triangles)//3))
        fw(struct.pack('%sI' % len(triangles), *triangles))

        if anim:
            write_animation(fw, obj_arm, ob.vertex_groups, global_matrix, ob.matrix_world)

        ob.to_mesh_clear()


def write_animation(fw, armature, v_groups, global_matrix, ob_matrix_world):
    fps = bpy.context.scene.render.fps
    fw(struct.pack("B", fps))

    arm_data = armature.data.copy()
    #arm_data.transform(global_matrix @ ob_matrix_world)
    arm_data.transform(global_matrix)
    # set to rest pose
    arm_data.pose_position = 'REST'
    
    # bind pose
    # some bones might not have any groups, so they will be removed
    print("bind pose")
    bones = get_animations.get_bind_pose(arm_data.bones, v_groups)
    num_bones = len(bones)

    fw(struct.pack("H", num_bones))

    for b in bones:
        fw(struct.pack("H", b[1]))

    for b in bones:
        mat = get_animations.get_matrix(b)
        v1 = mat[0]
        v2 = mat[1]
        v3 = mat[2]
        v4 = mat[3]
        fw(struct.pack("ffff", v1.x, v1.y, v1.z, v1.w))
        fw(struct.pack("ffff", v2.x, v2.y, v2.z, v2.w))
        fw(struct.pack("ffff", v3.x, v3.y, v3.z, v3.w))
        fw(struct.pack("ffff", v4.x, v4.y, v4.z, v4.w))

    # animations

    arm_data.pose_position = 'POSE'
    old_action = armature.animation_data.action
    old_frame_current = bpy.context.scene.frame_current

    fw(struct.pack("H", len(bpy.data.actions)))

    for action in bpy.data.actions:
        armature.animation_data.action = action
        f_range = action.frame_range

        name = action.name
        fw(struct.pack("B", len(name)))
        fw(bytes(name, "utf-8"))

        numFrames = f_range[1] - f_range[0]
        fw(struct.pack("H", int(numFrames)))

        for i in range(int(f_range[0]), int(f_range[1])):
            bpy.context.scene.frame_set(i)
            pose = armature.pose

            #bones = get_animations.get_pose(pose.bones, v_groups, global_matrix)

            print("animation pose")
            bones = get_animations.get_bind_pose(pose.bones, v_groups)
            
            for b in bones:
                mat = get_animations.get_matrix_2(b, global_matrix)
                #mat = get_animations.get_matrix_3(b)

                v1 = mat[0]
                v2 = mat[1]
                v3 = mat[2]
                v4 = mat[3]
                fw(struct.pack("ffff", v1.x, v1.y, v1.z, v1.w))
                fw(struct.pack("ffff", v2.x, v2.y, v2.z, v2.w))
                fw(struct.pack("ffff", v3.x, v3.y, v3.z, v3.w))
                fw(struct.pack("ffff", v4.x, v4.y, v4.z, v4.w))

    armature.animation_data.action = old_action
    bpy.context.scene.frame_current = old_frame_current
    bpy.data.armatures.remove(arm_data)
    pass


def save(context,
         filepath,
         *,
         use_mesh_modifiers=True,
         use_selection=True,
         global_matrix=None,
         path_mode='AUTO',
         version="AUTO",
         ):

    scene = context.scene

    # Exit edit mode before exporting, so current object states are exported properly.
    if bpy.ops.object.mode_set.poll():
        bpy.ops.object.mode_set(mode='OBJECT')

    if use_selection:
        objects = savable_objects(context.selected_objects)
    else:
        objects = savable_objects(scene.objects)

    if len(objects) > 255:
        raise "To many objects"

    mesh_objs = get_mesh_objects(objects, context, global_matrix, use_mesh_modifiers)

    major_ver, minor_ver, animations = select_version(version, mesh_objs)

    with open(filepath, "wb") as f:
        fw = f.write
        write_header(fw, major_ver, minor_ver)
        write_geometry(fw, animations, mesh_objs, global_matrix)

    return {'FINISHED'}
