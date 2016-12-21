#version 330
out vec4 color;

in vec4 theColor;

void main()
{
    color = vec4(theColor);
}