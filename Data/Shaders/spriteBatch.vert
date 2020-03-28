#version 330
layout(location=0)in vec2 vertex;// <vec2 pos, vec2 tex>
layout(location=1)in vec4 color;
layout(location=2)in vec2 uv;

out vec2 TexCoords;
out vec4 fragColor;

uniform mat4 projection;

void main()
{
    fragColor=color;
    gl_Position=projection*vec4(vertex.xy,0.,1.);
    TexCoords=uv;
}
