#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_UV;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

out vec2 texCoords;

void main()
{
	gl_Position = u_CameraMatrix * vec4(u_ModelMatrix * vec4(a_Pos, 1.0f));
    texCoords = a_UV;
}

#shader fragment
#version 330 core

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
    //FragColor = vec4(1.0);
    vec3 color = texture(u_Texture, texCoords).rgb;
    //vec3 color = vec3(1,0,0);
    FragColor = vec4(color.r, color.r, color.r, 1.0);
}