#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

// core
#include "core/camera.h"
#include "core/gui.h"
#include "core/main.h"
#include "core/world.h"
// entities
#include "entities/entity.h"
// player
#include "player/player.h"
// voxel
#include "voxel/voxel_chunk.h"
#include "voxel/voxel_grid.h"

using namespace godot;

void initialize_artisan_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
    // core
    ClassDB::register_class<Camera>();
    ClassDB::register_class<GUI>();
    ClassDB::register_class<Main>();
    ClassDB::register_class<World>();
    // entities
    ClassDB::register_class<Entity>();
    // player
    ClassDB::register_class<Player>();
    // voxel
    ClassDB::register_class<VoxelChunk>();
    ClassDB::register_class<VoxelGrid>();
}

void uninitialize_artisan_module(ModuleInitializationLevel p_level) {}

extern "C" {
GDExtensionBool GDE_EXPORT artisan_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_artisan_module);
    init_obj.register_terminator(uninitialize_artisan_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
}
