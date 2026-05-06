shader_type spatial;
render_mode cull_back;

global uniform float camera_angle;

global uniform sampler2D voxel_atlas : source_color, filter_nearest;
uniform float atlas_cols = 3;
uniform float atlas_rows = 4;

global uniform sampler2D voxel_table : source_color, filter_nearest;
uniform float table_cols = 6;
uniform float table_rows = 8;

varying vec3 normal;
varying vec4 custom;

void vertex() {
	vec3 world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0f)).xyz;
	VERTEX.x = VERTEX.x - sin(camera_angle) * (world_position.y);
	VERTEX.z = VERTEX.z - cos(camera_angle) * (world_position.y);
	
	normal = NORMAL;
	custom = INSTANCE_CUSTOM;
}

void fragment() {
    vec3 n = abs(normal);
	float face;
	if (n.x > 0.9) 
		face = normal.x > 0.0 ? 0.0 : 1.0;
	else if (n.y > 0.9) 
		face = normal.y > 0.0 ? 2.0 : 3.0;
	else if (n.z > 0.9)
		face = normal.z > 0.0 ? 4.0 : 5.0;
	
	float voxel_id = custom.r;
	float texture_id = texture(voxel_table, vec2((face + 0.5) / table_cols, (voxel_id + 0.5) / table_rows)).r;
	texture_id = round(texture_id);

    // convert flat tile_id into 2D grid position
    // col: which column (remainder after filling rows)
    // row: which row (how many full rows of atlas_cols fit)
    float col = mod(texture_id, atlas_cols);
    float row = floor(texture_id / atlas_cols);

    // how much UV space one tile takes up
    // e.g. 4 cols → each tile is 0.25 wide in UV space
    // e.g. 2 rows → each tile is 0.5 tall in UV space
    vec2 tile_size = vec2(
        1.0f / float(atlas_cols),
        1.0f / float(atlas_rows)
    );

    // top-left corner of this tile in UV space
    // e.g. col=1, row=1, tile_size=(0.25, 0.5) → offset=(0.25, 0.5)
    vec2 tile_offset = vec2(col, row) * tile_size;

    // UV from mesh goes 0→1 across the face
    // scale it down to fit inside one tile, then shift to tile's position
    // UV=(0,0) → top-left of tile, UV=(1,1) → bottom-right of tile
    vec2 final_uv = tile_offset + UV * tile_size;

    ALBEDO = texture(voxel_atlas, final_uv).rgb;
}
