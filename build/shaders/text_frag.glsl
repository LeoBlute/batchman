#version 450 core

out vec4 FragColor;

in vec2 vUV;
in float vAtlasIndex;

uniform sampler2D u_atlases[1];

void main()
{
	int index = int(vAtlasIndex);
	vec4 tex = texture(u_atlases[index], vUV);
	FragColor = vec4(1.0f, 1.0f, 1.0f, tex.r);
} 
