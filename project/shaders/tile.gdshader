shader_type spatial;
render_mode cull_back, blend_mix;

void fragment() {
	ALBEDO = COLOR.rgb;
}
