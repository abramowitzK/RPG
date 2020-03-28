#version 330
in vec3 pos;
in vec3 normal;
in vec2 uv;

out vec3 out_normal;
out vec2 out_uv;

uniform mat4 mvp;

void main(){
	gl_Position = mvp*vec4(pos.xyz, 1.0f);
	out_normal = normal;
	out_uv = uv;
}