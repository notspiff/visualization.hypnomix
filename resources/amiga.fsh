#version 120
#extension GL_EXT_gpu_shader4 : require

flat varying vec4 v_color;

void main(void) 
{   
	float r = floor(v_color.r * 15.0); 
	float g = floor(v_color.g * 15.0); 
	float b = floor(v_color.b * 15.0); 
	gl_FragColor = vec4(r/15.0, g/15.0, b/15.0, v_color.a);
}
