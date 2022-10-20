#version 300 es
precision mediump float;

// uniform
uniform vec4 uColor;
// out
out vec4 FragColor;

void main()
{
     FragColor = uColor;
}