shader_type spatial;
render_mode cull_back;

global uniform float camera_angle;

void vertex() {
	vec3 world_position = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	VERTEX.x = VERTEX.x - sin(camera_angle) * (world_position.y);
	VERTEX.z = VERTEX.z - cos(camera_angle) * (world_position.y);
}

void fragment() {
	ALBEDO = COLOR.rgb;
}
