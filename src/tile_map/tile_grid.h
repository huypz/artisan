#ifndef TILE_GRID_H
#define TILE_GRID_H

#include <godot_cpp/classes/node.hpp>

class TileGrid : public godot::Node {
    GDCLASS(TileGrid, godot::Node)

public:
    int width = 6;
    int height = 6;

public:
    TileGrid();

};

#endif
