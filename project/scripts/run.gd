@tool
extends EditorScript

var voxel_table: Image = Image.create(NUM_FACES, NUM_VOXELS, false, Image.FORMAT_RH)

const NUM_VOXELS = 8
const NUM_FACES = 6

enum Face { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK }

func _run():
	# grass
	create_voxel(1, {
		Face.RIGHT: 3,
		Face.LEFT: 3,
		Face.TOP: 3,
		Face.BOTTOM: 3,
		Face.FRONT: 3,
		Face.BACK: 3
	})
	
	# brown wall
	create_voxel(2, {
		Face.RIGHT: 0,
		Face.LEFT: 0,
		Face.TOP: 1,
		Face.BOTTOM: 0,
		Face.FRONT: 0,
		Face.BACK: 0
	})
	
	var err = voxel_table.save_exr("res://assets/textures/voxel/voxel_table.exr")
	print("save result: ", err)


func create_voxel(voxel_id: int, faces: Dictionary):
	for face in faces:
		var texture_id: int = faces[face]
		# half-precision float largest finite number
		voxel_table.set_pixel(face, voxel_id, Color(texture_id, 0, 0, 0))
		
