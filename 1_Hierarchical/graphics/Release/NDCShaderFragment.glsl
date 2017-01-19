#version 430

in vec2 UV;

uniform sampler2D gbPosition;
uniform sampler2D gbNormal;
uniform sampler2D gbAmbient;
uniform sampler2D gbIntensity;

void main()
{
	vec3 fragpos = texture(gbPosition, UV).rgb;
	vec3 normal = texture(gbNormal, UV).rgb;
	vec3 ambient = texture(gbAmbient, UV).rgb;
	vec3 intensity = texture(gbIntensity, UV).rgb;

	gl_FragColor = vec4(normal, 1);
//	gl_FragColor = texture(textureSampler, UV);//vec4(1.0f, 0.0f, 0.0f, 1.0f);
}