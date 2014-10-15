#version 120
#extension GL_EXT_gpu_shader4 : require

flat varying vec4 v_color;

void main(void) 
{    
	gl_FragColor = v_color;
}
