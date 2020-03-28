#version 330
in vec4 pos;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

void main(){
	gl_Position = p*v*m*pos;
}