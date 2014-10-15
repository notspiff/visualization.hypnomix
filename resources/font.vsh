#version 120

attribute vec4 pos;
attribute vec2 texpos;

uniform mat4 mvp;

varying vec2 vtexpos;

void main(void) {
    vtexpos = texpos;
    
    gl_Position = mvp * pos;
}
