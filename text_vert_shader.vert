#version 410 core
layout (location = 0) in vec3 pos; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 tex;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
//    gl_Position = projection * view * vec4(pos[0], pos[1], 0.0, 1.0);
    gl_Position = projection * view * model * vec4(pos, 1.0);
    TexCoords = tex;
}