#version 330

in vec3 Color;
uniform int flip;

uniform vec3 SolidColor = vec3(-1,-1,-1);

out vec4 Fragment;

void main()
{
	if(flip == 0)
		Fragment = vec4(Color ,1);
	else
		Fragment = vec4(1-Color, 1);
	
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
		if(flip == 0)
			Fragment = vec4(SolidColor, 1);
		else
			Fragment = vec4(1 - SolidColor, 1);

	return;
}