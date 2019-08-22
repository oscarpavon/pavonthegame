bl_info = {
    "name": "Pavon Engine Addon",
    "category": "Object",
}

import bpy


class PavonEngineAddon(bpy.types.Operator):
    """Pavon Engine Addons Test"""
    bl_idname = "object.pavon_engine_addon"
    bl_label = "Pavon Engine"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        print("Fuck yeah")
        #we need send info to Editor server to import models

        return {'FINISHED'}

addon_keymaps = []

def register():
    bpy.utils.register_class(PavonEngineAddon)
    
    wm = bpy.context.window_manager
    km = wm.keyconfigs.addon.keymaps.new(name='Object Mode', space_type='EMPTY')

    kmi = km.keymap_items.new(PavonEngineAddon.bl_idname, 'E', 'PRESS', shift=True, alt=True)

    addon_keymaps.append(km)

def unregister():
    bpy.utils.unregister_class(PavonEngineAddon)
    
     # handle the keymap
    wm = bpy.context.window_manager
    for km in addon_keymaps:
        wm.keyconfigs.addon.keymaps.remove(km)
    # clear the list
    addon_keymaps.clear()


if __name__ == "__main__":
    register()