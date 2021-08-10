#version 300 es
layout (location = 0) in vec3 v3Positon; // <vec3 pos, vec2 tex>
layout (location = 1) in vec2 v2Cood;   // <vec3 pos, vec2 tex>

// uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// out
out vec2 TexCoord;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(v3Positon.xyz, 1.0);
    TexCoord = v2Cood;
}