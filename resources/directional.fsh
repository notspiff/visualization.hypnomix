#version 120
#extension GL_EXT_gpu_shader4 : require

#define FLAT_SHADING 1

#ifdef FLAT_SHADING
flat varying vec4 v_color;
#else
varying vec4 v_color;
#endif

void main(void) 
{    
	gl_FragColor = v_color;
}
