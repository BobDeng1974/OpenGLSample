// vertex shader 
uniform vec3 lightpos; //传入光源的模型坐标吧 
uniform vec4 eyepos; 
  
varying vec3 lightdir; 
varying vec3 halfvec; 
varying vec3 norm; 
varying vec3 eyedir; 
  
attribute vec3 rm_Tangent; 
  
void main(void) 
{ 
   vec4 pos = gl_ModelViewMatrix * gl_Vertex; 
   pos = pos / pos.w; 
    
//把光源和眼睛从模型空间转换到视图空间 
   vec4 vlightPos = (gl_ModelViewMatrix * vec4(lightpos, 1.0)); 
   vec4 veyePos   = (gl_ModelViewMatrix * eyepos); 
    
   lightdir = normalize(vlightPos.xyz - pos.xyz); 
   vec3 eyedir = normalize(veyePos.xyz - pos.xyz); 
    
  //模型空间下的TBN 
   norm = normalize(gl_NormalMatrix * gl_Normal); 
  
   vec3 vtangent  = normalize(gl_NormalMatrix * rm_Tangent); 
  
   vec3 vbinormal = cross(norm,vtangent); 
    
   //将光源向量和视线向量转换到TBN切线空间 
   lightdir.x = dot(vtangent,  lightdir); 
   lightdir.y = dot(vbinormal, lightdir);  
   lightdir.z = dot(norm     , lightdir); 
   lightdir = normalize(lightdir); 
    
   eyedir.x = dot(vtangent,  eyedir); 
   eyedir.y = dot(vbinormal, eyedir); 
   eyedir.z = dot(norm     , eyedir); 
   eyedir = normalize(eyedir); 
    
   halfvec = normalize(lightdir + eyedir); 
  
   gl_FrontColor = gl_Color; 
    
   gl_TexCoord[0] = gl_MultiTexCoord0; 
    
   gl_Position = ftransform(); 
}