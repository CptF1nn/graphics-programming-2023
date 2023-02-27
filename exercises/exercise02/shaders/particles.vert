#version 330 core

layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in float Size;
layout (location = 2) in float Birth;
layout (location = 3) in float Duration;
layout (location = 4) in vec4 Color;
layout (location = 5) in vec2 Velocity;

out vec4 vertColor;

uniform float currentTime;
uniform float gravity;

void main()
{
	float age = currentTime - Birth;
	float scale = Duration - age;
	gl_PointSize = Size * scale;

	vec2 velAge = Velocity * age;
	vec2 gravAge = vec2(0.0, age * age * 0.5 * gravity);

	gl_Position = vec4(ParticlePosition + gravAge + velAge, 0.0, 1.0);
	vertColor = Color;
}
