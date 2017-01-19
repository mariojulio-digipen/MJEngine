#version 150

precision highp float;

in vec2 UV;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor.xyz = texture(textureSampler, UV).xyz;
}