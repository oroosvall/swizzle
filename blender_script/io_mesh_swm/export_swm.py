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

HEADER_MAGIC = "SWM "
UV_BIT = 0
NORMAL_BIT = 1
COLOR_BIT = 2

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

            vertex = (vpos, v_norm, v_uv, v_color)
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


def set_bit(value, bit):
    return value | (1<<bit)


def clear_bit(value, bit):
    return value & ~(1<<bit)


def is_bit_set(value, bit):
    return (value & (1<<bit)) != 0


def save(context,
         filepath,
         *,
         use_mesh_modifiers=True,
         use_selection=True,
         global_matrix=None,
         path_mode='AUTO',
         version="AUTO",
         ):

    with open(filepath, "wb") as f:
        fw = f.write
        fw(bytes(HEADER_MAGIC, "utf-8"))
        fw(struct.pack("BBBB", 1,0,0,0))

        scene = context.scene
        depsgraph = context.evaluated_depsgraph_get()

        # Exit edit mode before exporting, so current object states are exported properly.
        if bpy.ops.object.mode_set.poll():
            bpy.ops.object.mode_set(mode='OBJECT')

        if use_selection:
            objects = savable_objects(context.selected_objects)
        else:
            objects = savable_objects(scene.objects)

        if len(objects) > 255:
            raise "To many objects"

        fw(struct.pack("B", len(objects)))
        for ob in objects:
            fw(struct.pack("I", len(ob.name)))
            fw(bytes(ob.name, "utf-8"))
            flags = 0
            
            ob_for_convert = ob.evaluated_get(depsgraph) if use_mesh_modifiers else ob.original
            me = ob_for_convert.to_mesh()
            mesh_triangulate(me)

            me.transform(global_matrix)
            me.calc_normals_split()

            vertex_data, triangle_data = generate_vertex_data(me, True, True, False)

            verts = []
            normals = []
            uvs = []
            colors = []

            if me.uv_layers.active:
                flags = set_bit(flags, UV_BIT)

            if me.vertex_colors.active:
                flags = set_bit(flags, COLOR_BIT)

            flags = set_bit(flags, NORMAL_BIT)

            for key in vertex_data:
                p,n,u,c = vertex_data[key]
                verts.extend([*p])
                normals.extend([*n])
                if is_bit_set(flags, UV_BIT):
                    uvs.extend([*u])
                if is_bit_set(flags, COLOR_BIT):
                    colors.extend([*c])

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
                fw(struct.pack('%sf' % len(colors), *colors))

            fw(struct.pack("I", len(triangles)//3))
            fw(struct.pack('%sI' % len(triangles), *triangles))

            ob_for_convert.to_mesh_clear()

    return {'FINISHED'}
