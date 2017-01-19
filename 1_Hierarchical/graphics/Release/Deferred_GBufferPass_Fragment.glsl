#version 430
out vec3 gPosition;
out vec3 gNormal;
out vec3 gDiffuse;
out vec3 gIntensity;


in vec3 gFragPos;
in vec2 gUV; 
in vec3 gNormalVec;

uniform vec3 LightAmbient;
uniform vec3 LightIntensity;
// add shininess - alpha variable

uniform bool IsExtraActive;
uniform bool ExtraTextureColor;
uniform bool HasExtraTextures;

uniform sampler2D textureSampler;
uniform sampler2D extraSampler1;


void main()
{
	vec4 ExtraTextureColor = texture(extraSampler1, gUV) + vec4(LightAmbient, 1.0);

	vec3 ks = LightIntensity;
	vec3 kd;
	
	//if(HasExtraTextures == true && (ExtraTextureColor != vec4(LightAmbient, 1.0)) && IsExtraActive)
	//{
	//	kd = ExtraTextureColor.rgb + texture(textureSampler, gUV).rgb + LightAmbient;//
	//}
	//else
	//{
	//	kd = texture(textureSampler, gUV).rgb + LightAmbient;
	//}
	
	kd = texture(textureSampler, gUV).rgb + LightAmbient;
	gPosition = gFragPos;
	gNormal = normalize(gNormalVec);
	gDiffuse = kd;
	gIntensity = ks; // this is my specular!
	
	
	//gl_FragColor = gFragPos;
	

}