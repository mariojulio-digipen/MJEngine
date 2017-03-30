#version 430

precision highp float;

in vec2 UV;
in vec3 NormalVec;
in vec3 EyeVec;

in vec3 LightAmbient;
in vec3 LightIntensity;
in vec3 LightVec;
in vec3 LightColor;

uniform sampler2D waterSampler;

//flat in int InstanceID;

void main()
{
	float textureAlpha;

	vec3 N = normalize(NormalVec);
	vec3 L = normalize(LightVec);
	vec3 V = normalize(EyeVec);
	vec3 H = normalize(L+V);

	vec3 F;
	float G;
	float D;

	vec3 Kd, Ed;
	vec3 Ks, Er; 
	vec3 BRDF;

	float a, gloss; // gloss goes through     rough surface <-- [0..1] --> smooth surface
	float PI = 3.14159;
	float LN = max(0.0, dot(L, N));
	float NH = max(0.0, dot(N, H));
	float LH = max(0.0, dot(L, H));

	textureAlpha = texture(waterSampler, UV).a;

	Kd = texture(waterSampler, UV).rgb + LightAmbient;
	float n = 0.5;
	Ks = vec3(n, n, n) + LightColor;
	
	gloss = 0.2;
	a = pow(8192.0, gloss);

	// Diffuse
	Ed = Kd / PI;

	// Specular
	F = Ks + (1.0 - Ks) * pow(1.0 - LH, 5.0); // Fresnel term (Schlick approximation) 
	D = ((a + 2.0) / (2 * PI)) * pow(NH, a); // Distribution term
	G = 1 / pow(LH, 2.0); // Shadowing term
	Er = (F*D*G)/4.0;
	BRDF = Ed + Er;
	
	//Kd*0.2 + BRDF * LN * LightIntensity
	gl_FragColor.rgb = vec3(1.0f, 0.2f, 0.2f);//Kd*0.2 + BRDF * LN * LightIntensity/5;//vec3(0.0, 0.0, 1.0) * InstanceID;//
	gl_FragColor.a = textureAlpha;
	
}