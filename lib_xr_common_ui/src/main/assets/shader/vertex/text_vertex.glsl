#version 300 es
layout (location = 0) in vec3 v3Positon; // <vec3 pos, vec2 tex>
layout (location = 1) in vec2 v2Cood;   // <vec3 pos, vec2 tex>
out vec2 TexCoords;

// uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec4 uColor;

// outputs
out vec4 Color;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(v3Positon.xyz, 1.0);
    Color = uColor;
    TexCoords = v2Cood;
}