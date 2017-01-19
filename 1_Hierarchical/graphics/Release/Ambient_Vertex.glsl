#version 430

in vec3 Position; // bound to attribute index 0 


//out vec2 UV;

void main()
{
	vec4 vertex = vec4(Position, 1.0);
	//UV = 
	gl_Position = vertex;
}