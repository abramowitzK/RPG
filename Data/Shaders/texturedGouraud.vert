#version 330
in vec3 pos;
in vec3 normal;
in vec2 uv;

out vec4 out_color;
out float diffuse_out;
out vec2 uv_out;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 mvp;
vec3 LightPos = vec3(0.0, 1.0, 1.0);
vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
void main(){
	mat4 modelView = v*m;
	vec4 transformed = (m*vec4(pos,1.0));
	vec3 transformedVertex = vec3(transformed)/transformed.w;
	vec3 transformedNormal = normalize((m*vec4(normal,0.0)).xyz);
	float d = length(LightPos - transformedVertex);
	vec3 lightVector = normalize(LightPos);
	float diffuse = max(dot(transformedNormal, lightVector), 0.0);
	float attenuation = 1.0;
	diffuse = diffuse*attenuation;
	vec3 reflectDir = reflect(-lightVector, transformedNormal);
	vec3 viewDir = normalize(-transformedVertex);
	out_color = color*diffuse;
	diffuse_out = diffuse;
	uv_out = uv;
	gl_Position = mvp*vec4(pos,1.0);
}