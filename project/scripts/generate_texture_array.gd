@tool
extends EditorScript

const ATLAS_PATH: String = "res://assets/textures/environment/environment.png"
const TEXTURE_SIZE: int = 8
const OUTPUT: String = "res://assets/textures/environment/environment.res"

func _run():
	var atlas = Image.load_from_file(ATLAS_PATH)
	var cols = atlas.get_width() / TEXTURE_SIZE
	var rows = atlas.get_height() / TEXTURE_SIZE

	var images = []
	for row in rows:
		for col in cols:
			var region = Rect2i(col * TEXTURE_SIZE, row * TEXTURE_SIZE, TEXTURE_SIZE, TEXTURE_SIZE)
			var tile = atlas.get_region(region)
			images.push_back(tile)

	var tex_array = Texture2DArray.new()
	tex_array.create_from_images(images)
	ResourceSaver.save(tex_array, OUTPUT, ResourceSaver.FLAG_NONE)
