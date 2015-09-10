//fragment shader 
uniform float shiness; 
uniform vec4 ambient, diffuse, specular; 
  
uniform sampler2D bumptex; 
uniform sampler2D basetex; 
  
float amb = 0.2; 
float diff = 0.2; 
float spec = 0.6; 
  
varying vec3 lightdir; 
varying vec3 halfvec; 
varying vec3 norm; 
varying vec3 eyedir; 
  
void main(void) 
{ 
   vec3 vlightdir = normalize(lightdir); 
   vec3 veyedir = normalize(eyedir); 
  
   vec3 vnorm =   normalize(norm); 
   vec3 vhalfvec =  normalize(halfvec);   
    
   vec4 baseCol = texture2D(basetex, gl_TexCoord[0].xy);  
    
   //Normal Map里的像素normal定义于该像素的切线空间 
   vec3 tbnnorm = texture2D(bumptex, gl_TexCoord[0].xy).xyz; 
    
   tbnnorm = normalize((tbnnorm  - vec3(0.5))* 2.0);  
    
   float diffusefract =  max( dot(lightdir,tbnnorm) , 0.0);  
   float specularfract = max( dot(vhalfvec,tbnnorm) , 0.0); 
    
   if(specularfract > 0.0){ 
   specularfract = pow(specularfract, shiness); 
   } 
    
   gl_FragColor = vec4(amb * ambient.xyz * baseCol.xyz 
                 + diff * diffuse.xyz * diffusefract * baseCol.xyz 
                 + spec * specular.xyz * specularfract ,1.0); 
}