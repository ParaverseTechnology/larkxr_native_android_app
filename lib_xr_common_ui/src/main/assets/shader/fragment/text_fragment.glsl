#version 300 es
precision mediump float;
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
in vec4 Color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(Color) * sampled;
}