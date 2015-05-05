#version 120
#extension GL_ARB_geometry_shader4 : enable
#extension ARB_explicit_attrib_location : require

varying vec4 v_color;
varying vec4 frgclr;

void main() {
	int i;

	frgclr = v_color;
	for(i = 0; i < gl_VerticesIn; i++) {
	frgclr = v_color;
		gl_Position = gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
	
	for(i = 0; i < gl_VerticesIn; i++) {
	frgclr = v_color;
		gl_Position = gl_PositionIn[i] + vec4(-0.1, 0.0, 0.0, 0.0);
		EmitVertex();
	}
	EndPrimitive();
}
