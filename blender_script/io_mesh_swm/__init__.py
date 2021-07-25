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

bl_info = {
    "name": "Swizzle swm format",
    "author": "",
    "version": (0, 0, 1),
    "blender": (2, 91, 0),
    "location": "File > Import-Export",
    "description": "Export SWM mesh",
    "category": "Import-Export",
}

if "bpy" in locals():
    import importlib
    if "export_swm" in locals():
        importlib.reload(export_swm)


import bpy
from bpy.props import (
        BoolProperty,
        FloatProperty,
        StringProperty,
        EnumProperty,
        )
from bpy_extras.io_utils import (
        ImportHelper,
        ExportHelper,
        orientation_helper,
        path_reference_mode,
        axis_conversion,
        )

class SWM_PT_export_transform(bpy.types.Panel):
    bl_space_type = 'FILE_BROWSER'
    bl_region_type = 'TOOL_PROPS'
    bl_label = "Transform"
    bl_parent_id = "FILE_PT_operator"

    @classmethod
    def poll(cls, context):
        sfile = context.space_data
        operator = sfile.active_operator

        return operator.bl_idname == "EXPORT_SCENE_OT_swm"

    def draw(self, context):
        layout = self.layout
        layout.use_property_split = True
        layout.use_property_decorate = False  # No animation.

        sfile = context.space_data
        operator = sfile.active_operator

        layout.prop(operator, 'global_scale')
        layout.prop(operator, 'path_mode')
        layout.prop(operator, 'axis_forward')
        layout.prop(operator, 'axis_up')


class SWM_PT_export_geometry(bpy.types.Panel):
    bl_space_type = 'FILE_BROWSER'
    bl_region_type = 'TOOL_PROPS'
    bl_label = "Geometry"
    bl_parent_id = "FILE_PT_operator"

    @classmethod
    def poll(cls, context):
        sfile = context.space_data
        operator = sfile.active_operator

        return operator.bl_idname == "EXPORT_SCENE_OT_swm"

    def draw(self, context):
        layout = self.layout
        layout.use_property_split = True
        layout.use_property_decorate = False  # No animation.

        sfile = context.space_data
        operator = sfile.active_operator

        layout.prop(operator, 'use_selection')
        layout.prop(operator, 'use_mesh_modifiers')

        layout.prop(operator, 'version')


@orientation_helper(axis_forward='-Z', axis_up='Y')
class ExportSwm(bpy.types.Operator, ExportHelper):
    """Save a Swizzle Model File"""

    bl_idname = "export_scene.swm"
    bl_label = 'Export SWM'
    bl_options = {'PRESET'}

    filename_ext = ".swm"
    filter_glob: StringProperty(
            default="*.swm;",
            options={'HIDDEN'},
            )

    # context group
    use_selection: BoolProperty(
            name="Selection Only",
            description="Export selected objects only",
            default=False,
            )

    # object group
    use_mesh_modifiers: BoolProperty(
            name="Apply Modifiers",
            description="Apply modifiers",
            default=True,
            )

    global_scale: FloatProperty(
            name="Scale",
            min=0.01, max=1000.0,
            default=1.0,
            )

    version: EnumProperty(
            name="Version",
            description="",
            items=(('AUTO', "Auto", "Automatic veresion selection"),
                   ('1.0', "Version 1.0", "Save as version 1.0"),
                   ('2.0', "Version 2.0", "Save as version 2.0")
                   ),
            default="AUTO"
    )

    path_mode: path_reference_mode

    check_extension = True

    def execute(self, context):
        from . import export_swm

        from mathutils import Matrix
        keywords = self.as_keywords(ignore=("axis_forward",
                                            "axis_up",
                                            "global_scale",
                                            "check_existing",
                                            "filter_glob",
                                            ))

        global_matrix = (Matrix.Scale(self.global_scale, 4) @
                         axis_conversion(to_forward=self.axis_forward,
                                         to_up=self.axis_up,
                                         ).to_4x4())

        keywords["global_matrix"] = global_matrix
        return export_swm.save(context, **keywords)

    def draw(self, context):
        pass


def menu_func_export(self, context):
    self.layout.operator(ExportSwm.bl_idname, text="Swizzle model (.swm)")


classes = (
    ExportSwm,
    SWM_PT_export_transform,
    SWM_PT_export_geometry,
)


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

    for cls in classes:
        bpy.utils.unregister_class(cls)


if __name__ == "__main__":
    register()
