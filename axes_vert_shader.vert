#version 330
layout (location = 0) in vec3 position;

out vec4 theColor;

uniform vec4 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
//    gl_Position = view * model * vec4(position, 1.0f);
    gl_Position = projection * view * model *vec4(position, 1.0f);
//    gl_Position = vec4(position, 1.0f);
    theColor = color;

}
