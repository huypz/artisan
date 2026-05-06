#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

// entities
#include "entities/entity.h"
// game
#include "game/game.h"
#include "game/camera.h"
// player
#include "player/player.h"
// voxel
#include "voxel/chunk.h"



using namespace godot;

void initialize_artisan_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
    // entities
    ClassDB::register_class<Entity>();
    // game
    ClassDB::register_class<Game>();
    ClassDB::register_class<Camera>();
    // player
    ClassDB::register_class<Player>();
    // voxel
    ClassDB::register_class<Chunk>();
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
