#version 330 core

out vec4 FragColor;

in vec4 vertColor;

void main()
{
	float circleLength = length(((gl_PointCoord * 2) - vec2(1,1)));
	vec4 newColor = vec4(1, 1, 1, 1 - circleLength);
	FragColor = newColor * vertColor;
}
