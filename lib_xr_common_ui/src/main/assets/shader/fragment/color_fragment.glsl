#version 300 es
#define DISABLE_MULTIVIEW 1

// uniform
uniform vec4 uColor;
// out
out vec4 FragColor;

void main()
{
     FragColor = uColor;
}