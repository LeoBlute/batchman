#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_texture;

void main()
{
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    FragColor = texture(u_texture, TexCoord);
}
