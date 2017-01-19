#version 430

in vec3 Position; // bound to attribute index 0 
in vec2 TextureCoord; // bound to attribute index 1
in vec3 Normal; // bound to attribute index 2
in vec3 Tangent; // bound to attribute index 3

uniform mat4 ProjViewModel;
//uniform mat4 Projection;
//uniform mat4 View;
uniform mat4 ModelMatrix;

out vec2 UV; //texcords

void main()
{
	vec4 vertex = vec4(Position, 1.0);
	vec3 worldVertex = vec3(ModelMatrix * vertex);

	UV = TextureCoord;
	gl_Position = vertex;
	//gl_Position = ProjViewModel * vertex;
    //gl_Position = Projection * View * ModelMatrix * vertex;
}