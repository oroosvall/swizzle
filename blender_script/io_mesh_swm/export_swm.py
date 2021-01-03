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
            fw(struct.pack("H", 0))

            ob_for_convert = ob.evaluated_get(depsgraph) if use_mesh_modifiers else ob.original
            me = ob_for_convert.to_mesh()
            mesh_triangulate(me)

            me.transform(global_matrix)

            me_verts = me.vertices[:]
            fw(struct.pack("I", len(me_verts)))
            for v in me_verts:
                fw(struct.pack("fff", *v.co))

            fw(struct.pack("I", len(me.polygons)))
            for f in me.polygons:
                fw(struct.pack("III", *f.vertices))

            ob_for_convert.to_mesh_clear()

    return {'FINISHED'}
