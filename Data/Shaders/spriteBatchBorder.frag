#version 330
in vec2 TexCoords;
in vec4 fragColor;
out vec4 outColor;

uniform sampler2D myTexture;

void main() {
   float border_width = 0.02;
   float aspect = 1.0;
   float maxX = 1.0 - border_width;
   float minX = border_width;
   float maxY = maxX / aspect;
   float minY = minX / aspect;

   if (TexCoords.x < maxX && TexCoords.x > minX &&
       TexCoords.y < maxY && TexCoords.y > minY) {
     outColor = texture(myTexture, TexCoords);
   } else {
     outColor = vec4(0.0, 0.0, 0.0, 1.0);
   }
}