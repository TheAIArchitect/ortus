#version 330
layout (location = 0) in vec3 position;
//layout (location = 2) in vec2 texCoord;
//layout (location = 1) in vec4 color;
layout (location = 1) in vec3 normal;

//out vec2 TexCoord;
out vec4 theColor;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    theColor = color;
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    //TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
