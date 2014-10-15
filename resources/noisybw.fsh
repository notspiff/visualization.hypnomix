#version 120

varying vec4 v_color;

void main(void) 
{   
	float r = floor(v_color.r * 3.0); 
	float g = floor(v_color.g * 3.0); 
	float b = floor(v_color.b * 3.0); 
	float c = (r + g + b) / 9.0;
	gl_FragColor = vec4(c, c, c, v_color.a);
}
