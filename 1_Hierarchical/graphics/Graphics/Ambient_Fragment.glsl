#version 430

precision highp float;


uniform sampler2D gbAmbient;

uniform int width;
uniform int height;

void main()
{
	vec2 UV;
	UV = gl_FragCoord.xy / vec2(width, height);

	vec3 Diffuse = texture(gbAmbient, UV).rgb; //diffuse
	Diffuse = Diffuse * 0.05;

	gl_FragColor = vec4(Diffuse, 1.0f);
}