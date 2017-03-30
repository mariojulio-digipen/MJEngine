#version 430

in vec3 Position; // bound to attribute index 0 
in vec2 TextureCoord; // bound to attribute index 1
in vec3 Normal; // bound to attribute index 2
in vec3 Tangent; // bound to attribute index 3
in ivec4 BoneIDs; // bound to attribute index 4
in vec4 BoneWeights; // bound to attribute index 5
in mat4 InstanceMatrix; // bound to attribute 6

uniform vec3 LightPositionIn;
uniform vec3 LightAmbientIn;
uniform vec3 LightIntensityIn;
uniform vec3 LightColorIn;

out vec2 UV;
out vec3 NormalVec;
out vec3 EyeVec;

out	vec3 LightAmbient;
out	vec3 LightIntensity;
out	vec3 LightVec;
out	vec3 LightColor;

uniform mat4 Projection;
uniform mat4 View;

uniform vec3 CameraPos;

//flat out int InstanceID; 

void main()
{
	vec4 vertex = vec4(Position, 1.0f);
	UV = vec2(TextureCoord.s, 1.0 - TextureCoord.t);

	gl_Position = Projection * View * InstanceMatrix * vertex;


	// light properties
	NormalVec = mat3(InstanceMatrix) * Normal;

	vec3 worldVertex = vec3(InstanceMatrix * vertex);
	EyeVec = CameraPos - worldVertex;

	LightAmbient = LightAmbientIn;
	LightIntensity = LightIntensityIn;
	LightVec = LightPositionIn - worldVertex;
	LightColor = LightColorIn;

	//InstanceID = gl_InstanceID;

}