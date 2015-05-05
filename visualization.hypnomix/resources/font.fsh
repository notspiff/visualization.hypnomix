#version 120

varying vec2 vtexpos;
uniform sampler2D texture;
// layout(location = 0) out vec4 color; 

void main(void) {    
	vec4 color = texture2D(texture, vtexpos);
// color = vec4(1.0, 1.0, 1.0, 1.0);
	gl_FragColor = color;
}
