#version 430

in vec3 Position; // bound to attribute index 0 
in vec2 TextureCoord; // bound to attribute index 1

uniform mat4 ProjViewModel;
uniform mat4 ModelMatrix;

out vec2 UVx;
out vec3 worldVertex;

void main()
{
	vec4 vertex = vec4(Position, 1.0);
	//worldVertex = vec3(ModelMatrix * vertex);

	UVx = TextureCoord;
	gl_Position = ProjViewModel * vertex;
}