@tool
extends PanelContainer

signal brush_tile_id_changed(tile_id: int)

const TILE_SIZE = Vector2(8, 8)
const ATLAS_COLS = 3
const ATLAS_ROWS = 4
const DISPLAY_SIZE = Vector2(32, 32)

@onready var grid: GridContainer = $VBoxContainer/ScrollContainer/GridContainer

var atlas: Texture2D = preload("res://assets/textures/tiles/tile_atlas.png")

func _ready() -> void:
	for child in grid.get_children():
		child.queue_free()
	
	fill_palette()
	
func fill_palette() -> void:
	for row in range(ATLAS_ROWS):
		for col in range(ATLAS_COLS):
			var atlas_tex = AtlasTexture.new()
			atlas_tex.atlas = atlas
			atlas_tex.region = Rect2(
				col * TILE_SIZE.x,
				row * TILE_SIZE.y,
				TILE_SIZE.x,
				TILE_SIZE.y
			)
			atlas_tex.filter_clip = true
			
			var btn = TextureButton.new()
			btn.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
			btn.texture_normal = atlas_tex
			btn.custom_minimum_size = DISPLAY_SIZE
			btn.stretch_mode = TextureButton.STRETCH_SCALE
			btn.ignore_texture_size = true
			btn.pressed.connect(_on_tile_pressed.bind(col + row * ATLAS_COLS))
			grid.add_child(btn)

func _on_tile_pressed(tile_id: int) -> void:
	brush_tile_id_changed.emit(tile_id)
