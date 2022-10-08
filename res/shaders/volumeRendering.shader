#shader vertex
#version 330 core
layout(location = 0) in vec3 a_Pos;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_Transform;
uniform int u_TextNumRows;

void main()
{
	gl_Position = u_CameraMatrix * vec4(u_Transform * u_ModelMatrix * vec4(a_Pos, 1.0f));
}

#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
    vec3 color = vec3(0.5, 0.5, 0.5);
    
    FragColor = vec4(color, 1.0);
}