#version 330
in float diffuse_out;
in vec4 out_color;
in vec2 uv_out;
in vec3 normal_out;
in vec3 world_pos;
uniform sampler2D tex0;
uniform sampler2D tex1;
out vec4 color;
void main(){
	vec3 normal = normalize(normal_out);
	// Take the three texture samples
	vec4 grassx = texture(tex0, world_pos.yz); // Project along x axis
	vec4 grassy = texture(tex0, world_pos.xz); // Project along y axis
	vec4 grassz = texture(tex0, world_pos.xy); // Project along z axis
	float slope = 1.0 - normal.y;
	vec4 rockx = texture(tex1, world_pos.yz); // Project along x axis
	vec4 rocky = texture(tex1, world_pos.xz); // Project along y axis
	vec4 rockz = texture(tex1, world_pos.xy); // Project along z axis

	vec4 mixx,mixy,mixz;
	float blendAmount = 0.0;
	if((slope < 0.2) && (normal_out.y > 0)){
		blendAmount = 0.0;
	} else if ((slope > 0.2 )&& (slope < 0.7) && (normal_out.y > 0) ){
		blendAmount = slope;
	} else {
		blendAmount = 1.0;
	}
	mixx = mix(grassx, rockx, blendAmount);
	mixy = mix(grassy, rocky, blendAmount);
	mixz = mix(grassz, rockz, blendAmount);
	// Blend the samples according to the normal*/
	color = mixx * normal.x + mixy * normal.y + mixz * normal.z;
}