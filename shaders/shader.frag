#version 330 core

in vec2 texCoord;
in vec3 color;
in vec4 fragPosLightSpace;


uniform sampler2D tex;
uniform sampler2D shadowMap;

out vec4 fragColor;


void main() {
    //fragColor = texture(tex, texCoord);

   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;
   float closestDepth = texture(shadowMap, projCoords.xy).r;
   float currentDepth = projCoords.z;
   //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
   if(currentDepth > closestDepth)
   {
      fragColor = vec4(0);
   }else
   {

          fragColor = vec4(color,1.0);
   }

  //      fragColor = vec4(color,1.0);
}
