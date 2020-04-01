#version 330
in vec2 TexCoords;
in vec4 fragColor;
out vec4 outColor;

uniform sampler2D myTexture;

void main()
{
	vec4 sampled = texture(myTexture, TexCoords);
	outColor = sampled;
}
