#version 330
//in vec2 TexCoord;
/*
in vec4 theColor;

out vec4 color;

//uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;

void main()
{
    //color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
    color = theColor;
}
*/

out vec4 color;

in vec3 FragPos;
in vec3 Normal;
in vec4 theColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
//uniform vec3 objectColor;

void main()
{
    vec3 objectColor = theColor.xyz;
    // Ambient
    float ambientStrength = 0.4f;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.3f);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.4f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
//    color = vec4(result, 1.0f);
    color = vec4(result, theColor.w);
}