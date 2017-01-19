#version 430

in vec3 Position; // bound to attribute index 0 
in vec2 TextureCoord; // bound to attribute index 1
in vec3 Normal; // bound to attribute index 2
in vec3 Tangent; // bound to attribute index 3
in ivec4 BoneIDs; // bound to attribute index 4
in vec4 BoneWeights; // bound to attribute index 5

//------
//in vec3 DebugVertex; // bound to attribute index 3
//in vec3 DebugColor; // bound to attribute index 4
//------

out vec2 UV; //texcords
out vec3 NormalVec; //normalvec
//out vec3 LightVec;
out vec3 EyeVec;
//out vec4 FragPosition; //worldpos



//------
//out vec4 FragDebugVertex;
//out vec4 FragDebugColor;
//------
struct GLight
{
	vec3 LightPosition;
	vec3 LightAmbient;
	vec3 LightIntensity;
	vec3 LightColor;
};
const int GLIGHT_BUFFER = 5;
uniform GLight GLights[GLIGHT_BUFFER];

//uniform vec3 LightPosition;
uniform vec3 CameraPos;
uniform mat4 ProjViewModel;
uniform mat4 ModelMatrix;
uniform mat4 LineT;
uniform mat4 LineS;
uniform mat4 ClothS;


uniform int IsLine;
uniform int IsAnimMesh;
uniform int IsCloth;

uniform mat4 Projection;
uniform mat4 View;
//uniform mat4 OtherModel;
uniform mat4 VertexBonesWeights[100]; // don't forget to make the size dynamic, how? Don't know yet :\


out float id1;
out float id2;
out float id3;
out float id4;

out float weight1;
out float weight2;
out float weight3;
out float weight4;

struct FLight
{
	vec3 LightAmbient;
	vec3 LightIntensity;
	vec3 LightVec;
	vec3 LightColor;
};
out FLight Fraglights[GLIGHT_BUFFER];


void main(void) 
{
		vec4 vertex = vec4(Position, 1.0);
		UV = vec2(TextureCoord.s, 1.0 - TextureCoord.t);

		
		
		

		mat4 DeformTransform = mat4(1.0f);
		if(IsAnimMesh == 1)
		{
			mat4 vertexBoneMat1 = mat4(1.0f);
			mat4 vertexBoneMat2 = mat4(1.0f);
			mat4 vertexBoneMat3 = mat4(1.0f);
			mat4 vertexBoneMat4 = mat4(1.0f);
			if(BoneIDs.x != 10000)
				vertexBoneMat1 = VertexBonesWeights[BoneIDs.x];
			if(BoneIDs.y != 10000)
				vertexBoneMat2 = VertexBonesWeights[BoneIDs.y];
			if(BoneIDs.z != 10000)
				vertexBoneMat3 = VertexBonesWeights[BoneIDs.z];
			if(BoneIDs.w != 10000)
				vertexBoneMat4 = VertexBonesWeights[BoneIDs.w];


			DeformTransform =  vertexBoneMat1 * BoneWeights.x;
			DeformTransform += vertexBoneMat2 * BoneWeights.y;
			DeformTransform += vertexBoneMat3 * BoneWeights.z;
			DeformTransform += vertexBoneMat4 * BoneWeights.w;


			id1 = float(BoneIDs.x);
			id2 = float(BoneIDs.y);
			id3 = float(BoneIDs.z);
			id4 = float(BoneIDs.w);

			weight1 = BoneWeights.x;
			weight2	= BoneWeights.y;
			weight3	= BoneWeights.z;
			weight4	= BoneWeights.w;


			//DeformTransform = mat4(
			//vec4(1.0f, 0.0f, 0.0f, 0.0f),
			//vec4(0.0f, cos(radians(45.0f)), -sin(radians(45.0f)), 0.0f),
			//vec4(0.0f, sin(radians(45.0f)), cos(radians(45.0f)), 0.0f),
			//vec4(1.02555f, -141.9389f, -1.3856f, 1.0f));

			//DeformTransform = mat4(
			//vec4(1.0f, 0.0f, 0.0f, 0.0f),
			//vec4(0.0f, 1.0f, 0.0f, 0.0f),
			//vec4(0.0f, 0.0f, 1.0f, 0.0f),
			//vec4(1.02555f, -141.9389f, -1.3856f, 1.0f));

			//if( vertexBoneMat1)

			NormalVec = mat3(ModelMatrix) * mat3(DeformTransform) * Normal;
			gl_Position =  ProjViewModel * (DeformTransform) * vertex;
		}else
		{
			NormalVec = mat3(ModelMatrix) * Normal;
			if(IsLine == 1)
			{
				mat4 model = LineT*LineS;
				gl_Position = Projection * View * model * vertex;
			}
			else
			{
				 //put hatching vertex stuff here
				if(IsCloth == 1)
				{
					mat4 model = ClothS;
					gl_Position = Projection * View * model * vertex;

				}
				else
				{
					gl_Position = ProjViewModel * vertex;
				}
				
				
			}
		}

		
		vec3 worldVertex = vec3(ModelMatrix * vertex);

		//#pragma optionNV(unroll)
		for(int i=1; i<GLIGHT_BUFFER; i++)
		{
			//if(i != 3) continue;
			Fraglights[i].LightAmbient = GLights[i].LightAmbient;
			Fraglights[i].LightIntensity = GLights[i].LightIntensity;
			Fraglights[i].LightVec = GLights[i].LightPosition - worldVertex;
			Fraglights[i].LightColor = GLights[i].LightColor;
		}
		//LightVec = LightPosition - worldVertex;
		
		
		EyeVec = CameraPos - worldVertex;
		
	
}