#version 150

in vec3 Position; // bound to attribute index 0 
in vec2 TextureCoord; // bound to attribute index 1

out vec2 UV;

uniform mat4 ProjViewModel;



void main(void) 
{
	vec4 vertex = vec4(Position, 1.0);
	gl_Position = ProjViewModel * vertex;

	UV = TextureCoord;
}