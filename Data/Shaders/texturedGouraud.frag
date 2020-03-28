#version 330
in float diffuse_out;
in vec4 out_color;
in vec2 uv_out;
uniform sampler2D myTexture;
out vec4 color;
void main(){
	color = texture(myTexture, uv_out)*diffuse_out + 0.3*texture(myTexture, uv_out);
}