#version 430

precision highp float;

//out vec4 FragColor;


uniform sampler2D gbPosition;
uniform sampler2D gbNormal;
uniform sampler2D gbAmbient;
uniform sampler2D gbIntensity;

uniform sampler2D textureSampler;

uniform	vec3 LightPosition;
uniform	vec3 LightAmbient;
uniform	vec3 LightIntensity;
uniform	vec3 LightColor;

uniform int height;
uniform int width;
uniform int Lights;
uniform float ObjectScale;

in vec2 UVx;
in vec3 worldVertex;

uniform vec3 CameraPos;

vec4 computeLighting(float scale, vec3 fragpos, vec3 normal, vec3 ambient, vec3 intensity, vec3 camerapos);


void main()
{
	vec2 UV;
	UV = gl_FragCoord.xy / vec2(width, height);

	vec3 FragPos = texture(gbPosition, UV).rgb;
	vec3 Normal = texture(gbNormal, UV).rgb;
	vec3 Diffuse = texture(gbAmbient, UV).rgb; //diffuse
	vec3 Specular = texture(gbIntensity, UV).rgb; //specular



	vec4 color;
	color = computeLighting(ObjectScale, FragPos, Normal, Diffuse, Specular, CameraPos);
	gl_FragColor = color; //vec4(FragPos, 1);// vec4(1.0f, 0.0f, 0.0f, 1.0f);//
}



vec4 computeLighting(float scale, vec3 fragpos, vec3 normal, vec3 ambient, vec3 intensity, vec3 camerapos)
{
	//vec4 ExtraTextureColor = 
	//	texture(extraSampler1, UV) + vec4(Fraglights[lightIndex].LightAmbient, 1.0);
	float textureAlpha;


	// list of variables
	// construct variables made from deferred lighting
	vec3 LightVec = LightPosition - fragpos;
	vec3 distvector = LightPosition - fragpos;
	
	
	float distanceToLight = length(distvector);
	//float radius = scale *.5;
	float lightRange = scale*0.5;
	if(distanceToLight >= lightRange)
	{
		return vec4(0.0f,0.0f,0.0f, 0.0f);
	}
	//float exponentialFactor = (intensity/(distanceToLight*distanceToLight)) - (intensity/(radius*radius));
	//if(distanceToLight > radius)
	//{
	//	return vec4(0.0f,0.0f,0.0f, 1.0f);
	//}
	
		
	
	
	vec3 EyeVec = camerapos - fragpos;
	vec3 myFragColor;
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
	gloss = 0.2;
	textureAlpha = 1.0f;//texture(textureSampler, UV).a;
	//if(textureSize(textureSampler,0).x > 1) // Is texture defined?
	//{
		//Kd = Diffuse + Fraglights[lightIndex].LightAmbient;
		Kd = ambient + LightAmbient;
	//}

	float n = 0.5; // refractive index
	float f0 = pow(((1-n)/(1+n)),2);

	//Ks = vec3(0.04, 0.04, 0.04);
	//float n = 0.5f;
	Ks = /*vec3(f0, f0, f0) +*/ LightColor;
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



	//if(distanceToLight < radius)//3.0f)
	//	da_color = Kd*exponentialFactor*0.0005 + BRDF * LN * LightIntensity; //vec3(distanceToLight,distanceToLight,distanceToLight) / 10;// / (distanceToLight*distanceToLight + 1);//Kd*exponentialFactor*0.0005 + BRDF * LN * LightIntensity;
	//else
	//	da_color = vec3(0.0f,0.0f,0.0f);
	
	//da_color = Kd*exponentialFactor*0.0005*LightColor + BRDF * LN * LightIntensity;
	da_color = LightColor*LN*BRDF*LightIntensity;
	if(Lights == 1)
	{
		//if(HasExtraTextures == true && (ExtraTextureColor != vec4(GLights[lightIndex].LightAmbient, 1.0)) && IsExtraActive)
		//{
		//	myFragColor = ExtraTextureColor+vec4(da_color, textureAlpha);//TextureColor * (D+A+S);
		//}
		//else
		//{
			//myFragColor = vec4(Kd*exponentialFactor*0.0005*LightColor, 1.0f);
			//myFragColor = vec4(da_color, textureAlpha)/* * 0.5*/;//vec4(clamp(Diffuse, 0.0f, 1.0f) + AmbientLight + clamp(Specular, 0.0f ,1.0f), 1.0);
		//}
		myFragColor = vec3((1-distanceToLight/lightRange)) * vec3((1-distanceToLight/lightRange)) * 
			vec3((1-distanceToLight/lightRange)) * vec3((1-distanceToLight/lightRange)) * da_color;	
	}
	//if(distanceToLight > 5)
	//{
	//	return vec4(0.0f, 0.0f, 0.0f, 0.0f);
	//}
	vec4 finalcolor = vec4(myFragColor, 0.5f);

	return myFragColor; //vec4(distanceToLight);//
}