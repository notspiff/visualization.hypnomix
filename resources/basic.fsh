#version 120


#ifdef FLAT_SHADING
#extension GL_EXT_gpu_shader4 : require
flat varying vec4 v_color;
#else
varying vec4 v_color;
#endif

void main(void) 
{    
	gl_FragColor = v_color;
}
