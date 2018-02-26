 #version 330 core

in vec3 FragPos; 
in vec3 Normal;
in vec2 TextCoords;  
out vec4 finalColor;


uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform sampler2D TEXTURE;

void main()
{
	vec3 lightColor=vec3(1.0f,1.0f,1.0f);
	vec3 colorTexture = texture(TEXTURE, TextCoords).rgb;
    float constantAmbient = 0.3f;
    vec3 ambient = constantAmbient * lightColor *colorTexture;//*colorTexture
  	
    // Diffuse 
    vec3 normale = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normale, lightDir), 0.0);
    vec3 diffuse = diff * lightColor*colorTexture;
    
    
   // specular
   float specularIntensity = 0.3f;
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 halfwayDir = normalize(lightDir + viewDir);  
   float spec = pow(max(dot(normale, halfwayDir), 0.0), 32); 
   vec3 specular = specularIntensity * spec * lightColor;
     
    vec3 result = (ambient + diffuse + specular);
   	
   	finalColor = vec4(result, 1.0f);
}