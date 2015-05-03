#version 120
#extension GL_EXT_geometry_shader4 : require
#extension GL_EXT_gpu_shader4 : require

flat varying vec4 v_color;
flat varying vec4 fv_color;

void main(void) {
	int i;

	for(i = 0; i < gl_VerticesIn; i++) {
		gl_Position = gl_PositionIn[i];
		EmitVertex();
		fv_color = max(vec4(1.0, 1.0, 1.0, 1.0), v_color); 
	}
	EndPrimitive();
}
