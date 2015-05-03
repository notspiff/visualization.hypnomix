#version 120

varying vec4 v_color;
varying vec2 v_texpos;
uniform sampler2D texture0;

void main(void) 
{    
	vec4 clr = texture2D(texture0, gl_PointCoord);
//	if(clr.r == 0.0 && clr.g == 0.0 && clr.b == 0.0) {
//		discard;
//	}
//	gl_FragColor = v_color * clr;
	gl_FragColor = v_color;
}
