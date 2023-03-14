#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture1;
uniform vec2 ColorTextureScale1;
uniform sampler2D ColorTexture2;
uniform vec2 ColorTextureScale2;
uniform sampler2D ColorTexture3;
uniform vec2 ColorTextureScale3;
uniform sampler2D ColorTexture4;
uniform vec2 ColorTextureScale4;

void main()
{
	FragColor = Color * texture(ColorTexture1, TexCoord * ColorTextureScale1);
}
