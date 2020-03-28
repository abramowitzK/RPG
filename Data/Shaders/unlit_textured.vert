#version 330
in vec4 pos;
in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 mvp;

void main(){
	gl_Position = mvp*pos;
	TexCoord = texCoord;
}