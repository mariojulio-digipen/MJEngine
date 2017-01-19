#version 430

precision highp float;

//out vec4 FragColor;


uniform sampler2D gbPosition;
uniform sampler2D gbNormal;
uniform sampler2D gbAmbient;
uniform sampler2D gbIntensity;

struct GLight
{
	vec3 LightPosition;
	vec3 LightAmbient;
	vec3 LightIntensity;
	vec3 LightColor;
};
const int NUM_LIGHTS = 1000;
uniform GLight GLights[NUM_LIGHTS];
uniform vec3 CameraPos;
uniform int Lights;

uniform int height;
uniform int width;
uniform int lightnum;

//in vec2 UV;


vec4 computeLighting(int lightIndex, vec3 fragpos, vec3 normal, vec3 ambient, vec3 intensity, vec3 camerapos);

void main()
{
	vec2 UV;
	UV = gl_FragCoord.xy / vec2(width, height);

	vec3 FragPos = texture(gbPosition, UV).rgb;
	vec3 Normal = texture(gbNormal, UV).rgb;
	vec3 Diffuse = texture(gbAmbient, UV).rgb; //diffuse
	vec3 Specular = texture(gbIntensity, UV).rgb; //specular


	//if(IsLine == 1)
	//{
	//	gl_FragColor = vec4(0.0f, 0.0, 1.0f, 1.0f);
	//	return;
	//}

	vec4 color;

	if(Lights == 0)
	{
		//if(DebugMode == 1)
		//{
		//	color = vec4(0.0, 0.0, 0.0, 1.0);
		//}	
		//else
		//{
			//color = vec4(0.0, 1.0, 0.0, 1.0);
			//color = texture(textureSampler, UV);
		//}
		
	}else
	{
		//#pragma optionNV(unroll)
		for(int i=1; i<lightnum; i++)
		{
			//if(i != 3) continue;
			color += computeLighting(i, FragPos, Normal, Diffuse, Specular, CameraPos);
		}
	}


	gl_FragColor = color;

}



vec4 computeLighting(int lightIndex, vec3 fragpos, vec3 normal, vec3 ambient, vec3 intensity, vec3 camerapos)
{
	//vec4 ExtraTextureColor = 
	//	texture(extraSampler1, UV) + vec4(Fraglights[lightIndex].LightAmbient, 1.0);
	float textureAlpha;


	// list of variables
	// construct variables made from deferred lighting
	vec3 LightVec = GLights[lightIndex].LightPosition - fragpos;
	
	vec3 EyeVec = camerapos - fragpos;
	vec4 myFragColor;
	//float attenuation = 1.0f / (1.0f + 0.01f*distanceToLight + 0.001f*pow(distanceToLight, 2));
	//float attenuation = clamp(1.0 - distanceToLight*distanceToLight/(5*5), 0.0, 1.0);
	//attenuation *= attenuation;

	vec3 N = normalize(normal);//Fraglights
	//vec3 L = normalize(LightVec);
	vec3 L = normalize(LightVec);//Fraglights[lightIndex].LightVec); // GLight[lightIndex].LightPosition - fragpos
	vec3 V = normalize(EyeVec);
	
	vec3 H = normalize(L+V);

	vec3 F; // fresnel term
	float G; // geometry term
	float D; // distribution term


	vec3 Kd, Ed;
	vec3 Ks, Er; 
	vec3 BRDF;
	vec3 da_color;
	float a, gloss; // gloss goes through     rough surface <-- [0..1] --> smooth surface
	float PI = 3.14159;
	float LN = max(0.0, dot(L, N));
	float NH = max(0.0, dot(N, H));
	float LH = max(0.0, dot(L, H));

	// micro-facet BRDF at a point on a surface
	gloss = 0.5;
	textureAlpha = 1.0f;//texture(textureSampler, UV).a;
	//if(textureSize(textureSampler,0).x > 1) // Is texture defined?
	//{
		//Kd = Diffuse + Fraglights[lightIndex].LightAmbient;
		Kd = ambient + GLights[lightIndex].LightAmbient;
	//}

	float n = 0.5; // refractive index
	float f0 = pow(((1-n)/(1+n)),2);

	//Ks = vec3(0.04, 0.04, 0.04);
	//float n = 0.5f;
	Ks = vec3(f0, f0, f0) + GLights[lightIndex].LightColor;
	//Ks = Specular + Fraglights[lightIndex].LightColor;
	a = pow(8192.0, gloss);


	// Diffuse
	Ed = Kd / PI;

	// Specular
	F = Ks + (1.0 - Ks) * pow(1.0 - LH, 5.0); // Fresnel term (Schlick approximation) 
	D = ((a + 2.0) / (2 * PI)) * pow(NH, a); // Distribution term
	G = 1 / pow(LH, 2.0); // Shadowing term
	Er = (F*D*G)/4.0;

	// BRDF
	BRDF = /*attenuation **/ (Ed + Er);
	da_color = Kd*intensity + BRDF * LN * GLights[lightIndex].LightIntensity;
	//LightColor
	//da_color = GLights[lightIndex].LightColor*LN*BRDF;

	if(Lights == 1)
	{
		//if(HasExtraTextures == true && (ExtraTextureColor != vec4(GLights[lightIndex].LightAmbient, 1.0)) && IsExtraActive)
		//{
		//	myFragColor = ExtraTextureColor+vec4(da_color, textureAlpha);//TextureColor * (D+A+S);
		//}
		//else
		//{
			//myFragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			myFragColor = vec4(da_color, textureAlpha)/* * 0.5*/;//vec4(clamp(Diffuse, 0.0f, 1.0f) + AmbientLight + clamp(Specular, 0.0f ,1.0f), 1.0);
		//}
			
	}
	

	return myFragColor;
}