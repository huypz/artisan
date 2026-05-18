extends Control

signal brush_tile_id_changed(tile_id: int)

@onready var palette = $CanvasLayer/PanelContainer

func _ready() -> void:
	palette.brush_tile_id_changed.connect(_on_brush_tile_id_changed)
	
func _on_brush_tile_id_changed(tile_id: int) -> void:
	brush_tile_id_changed.emit(tile_id)
