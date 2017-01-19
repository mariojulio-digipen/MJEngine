#version 430

in vec3 Position; // bound to attribute index 0 
in vec2 TextureCoord; // bound to attribute index 1
in vec3 Normal; // bound to attribute index 2
in vec3 Tangent; // bound to attribute index 3

out vec2 gUV;
out vec3 gFragPos;
out vec3 gNormalVec;


uniform mat4 ProjViewModel;
uniform mat4 ModelMatrix;

uniform vec3 CameraPos;

void main()
{
	vec4 vertex = vec4(Position, 1.0);
	gUV = vec2(TextureCoord.s, 1.0 - TextureCoord.t);
	gFragPos = (ModelMatrix * vertex).xyz;
	//gFragPos = (ProjViewModel * vertex).xyz;
	gNormalVec = Normal;

	vec3 dummy = CameraPos;
	//gFragPos =  (ProjViewModel * vertex).xyz;
	gl_Position = ProjViewModel * vertex;
}