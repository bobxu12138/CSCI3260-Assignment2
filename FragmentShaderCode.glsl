#version 330 core
in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec4 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

uniform vec3 lightDirectionWorld;
uniform vec3 ambientLight1;
uniform sampler2D myTextureSampler0;
out vec4 color ;


void main()
{   vec4 MaterialAmbientColor =texture(myTextureSampler0 ,UV).rgba;
    vec4 MaterialDiffuseColor =texture(myTextureSampler0 ,UV).rgba;
    vec3 lightcolor = vec3(0.3,0.3,0.3);

    //spot light
    float distance1 = length(lightPositionWorld-vertexPositionWorld);
    float attentuation = 1.0/(1.0f+0.09f*distance1+0.032f*(distance1*distance1));
    vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
    float brightness = dot(lightVectorWorld, normalize(normalWorld));
    vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);
    vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
    vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
    float s =clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
    s = pow(s, 50);
    vec4 specularLight = vec4(s, 0, 0, 1);
    vec4  fragmentcolor0= ambientLight*MaterialAmbientColor+
    attentuation * MaterialDiffuseColor * clamp(diffuseLight,0,1)*0.5f
    +attentuation * vec4(lightcolor,1.0f) * specularLight*0.3f;
  
    //directional light
    vec3 lightDir = normalize(-lightDirectionWorld);
    float diff = max(dot(lightVectorWorld, normalize(normalWorld)),0);
    vec3 reflectDir =reflect(-lightDir,normalWorld);
    float spec=pow(max(dot(eyeVectorWorld,reflectDir),0.0),50);
    vec3 ambient =ambientLight1*(texture(myTextureSampler0 ,UV).rgb);
    vec3 diffuse =vec3(0.5f,0.5f,0.5f)*diff*(texture(myTextureSampler0 ,UV).rgb);
    vec3 specular = vec3(0.3f,0.3f,0.3f)*spec*(lightcolor);
    vec4 fragmentcolor =vec4((ambient+diffuse+specular),1.0f);
    
      
    color =fragmentcolor0 + fragmentcolor;
   
}
