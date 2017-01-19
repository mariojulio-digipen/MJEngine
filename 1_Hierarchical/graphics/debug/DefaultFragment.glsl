#version 430

precision highp float;

//uniform Lights[LIGHT_BUFFER];
struct FLight
{
	vec3 LightAmbient;
	vec3 LightIntensity;
	vec3 LightVec;
	vec3 LightColor;
};
const int FLIGHT_BUFFER = 5;
in FLight Fraglights[FLIGHT_BUFFER];

in vec2 UV;
in vec3 NormalVec;
in vec3 EyeVec;
//in vec3 LightVec;


in float id1;
in float id2;
in float id3;
in float id4;

in float weight1;
in float weight2;
in float weight3;
in float weight4;

//uniform vec3 EyePos;
//uniform vec3 LightPosition; //delete this

//uniform vec3 LightAmbient;
//uniform vec3 LightIntensity;

// list of flags
uniform int Transparent;
uniform int Lights;
uniform int DebugMode;
uniform bool HasExtraTextures;
uniform bool IsExtraActive;

uniform int IsLine;
uniform int IsAnimMesh;
uniform int IsDot;
uniform vec3 LineColor;
uniform int Selected;

//----
in vec4 FragDebugColor;


uniform sampler2D textureSampler;
uniform sampler2D extraSampler1;
uniform sampler2D extraSampler2;
//----

vec4 computeLighting(int lightIndex);

void main()
{

	//if(IsAnimMesh == 1)
	//{
	//	gl_FragColor = vec4(weight1, weight2, weight3, 1.0f);
	//	return;
	//}


	if(DebugMode == 1)
	{
		gl_FragColor = vec4(0.0f, 1.0, 1.0f, 1.0f);
		return;
	}
	
	if(IsLine == 1)
	{
		gl_FragColor = vec4(LineColor, 1.0f);
		return;
	}

	//if(IsDot == 1)
	//{
	//	gl_FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	//	return;
	//}

	vec4 color;

	if(Lights == 0)
	{
		if(DebugMode == 1)
		{
			color = vec4(0.0, 0.0, 0.0, 1.0);
		}	
		else
		{
			//color = vec4(0.0, 1.0, 0.0, 1.0);
			color = texture(textureSampler, UV);
		}
		
	}else
	{
		for(int i=1; i<FLIGHT_BUFFER; i++)
		{
			//if(i != 3) continue;
			color += computeLighting(i);
		}
	}

	if(Selected == 1)
		gl_FragColor = color * vec4(0.0f, 1.0f, 0.0f, 1.0f);
	
	if(Selected == 0)
		gl_FragColor = color;
	
}


vec4 computeLighting(int lightIndex)
{
	vec4 ExtraTextureColor = texture(extraSampler1, UV) + vec4(Fraglights[lightIndex].LightAmbient, 1.0);
	float textureAlpha;


	// list of variables

	vec3 N = normalize(NormalVec);//Fraglights
	//vec3 L = normalize(LightVec);
	vec3 L = normalize(Fraglights[lightIndex].LightVec);
	vec3 V = normalize(EyeVec);
	
	vec3 H = normalize(L+V);
	vec3 F;
	float G;
	float D;


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
	textureAlpha = texture(textureSampler, UV).a;
	if(textureSize(textureSampler,0).x > 1) // Is texture defined?
	{
		Kd = texture(textureSampler, UV).rgb + Fraglights[lightIndex].LightAmbient;
	}

	float n = 0.5; // refractive index
	float f0 = pow(((1-n)/(1+n)),2);

	//Ks = vec3(0.04, 0.04, 0.04);
	Ks = vec3(n, n, n) + Fraglights[lightIndex].LightColor;// * Fraglights[lightIndex].LightColor;
	a = pow(8192.0, gloss);


	// Diffuse
	Ed = Kd / PI;

	// Specular
	F = Ks + (1.0 - Ks) * pow(1.0 - LH, 5.0); // Fresnel term (Schlick approximation) 
	D = ((a + 2.0) / (2 * PI)) * pow(NH, a); // Distribution term
	G = 1 / pow(LH, 2.0); // Shadowing term
	Er = (F*D*G)/4.0;

	// BRDF
	BRDF = Ed + Er;
	da_color = Kd*0.2 + BRDF * LN * Fraglights[lightIndex].LightIntensity;

	vec4 myFragColor;

	if(Lights == 1)
	{
		if(HasExtraTextures == true && (ExtraTextureColor != vec4(Fraglights[lightIndex].LightAmbient, 1.0)) && IsExtraActive)
		{
			myFragColor = ExtraTextureColor+vec4(da_color, textureAlpha);//TextureColor * (D+A+S);
		}
		else
		{
			//myFragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			myFragColor = vec4(da_color, textureAlpha) * 0.5;//vec4(clamp(Diffuse, 0.0f, 1.0f) + AmbientLight + clamp(Specular, 0.0f ,1.0f), 1.0);
		}
			
	}
	

	return myFragColor;
}



//ALL THIS CODE IS JUST IN CASE I ROLL BACK PREVIOUS FEATURES!!!!!!!!!!!!!!!!!
//if(Transparent == 2)
	//{
	//	gl_FragColor = vec4(1,1,0,0.3);
	//}
	//if(Transparent == 3)
	//{
	//	gl_FragColor = vec4(1,0,0,0.5);
	//}

	//if(Transparent == 1)
	//{
	//	gl_FragColor = /*texture(textureSampler, UV).xyz * */ FragColor ;
	//	if(gl_FragColor.a != 0.0)
	//		gl_FragColor.a = 0.1;
	//}
	//else
	//{
	//	gl_FragColor = texture(textureSampler, UV) *  FragColor ;
	//}

	
	//if(IsText == 1)
	//{
	//	vec3 tmp = gl_FragColor.xyz;
	//	if(tmp.r >= 0 && tmp.r <= 0.17)
	//	{
	//		gl_FragColor.a = 0.0;
	//	}else
	//	{
	//		gl_FragColor = texture(textureSampler, UV);
	//	}
		
	//}