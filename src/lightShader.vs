#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TextCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	FragPos = vec3(model * vec4(position.x,position.y,position.z, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;  
	TextCoords = vec2(textCoords.x, textCoords.y);
}