#version 450 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aAtlasIndex;

uniform mat4 u_proj;

out vec2 vUV;
out float vAtlasIndex;

void main()
{
	vUV = aUV;
	vAtlasIndex = aAtlasIndex;
	gl_Position = u_proj * vec4(aPos, 1.0, 1.0);
}
