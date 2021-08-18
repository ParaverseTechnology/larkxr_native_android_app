#version 300 es
#define DISABLE_MULTIVIEW 1

layout (location = 0) in vec3 aPos;

// uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}