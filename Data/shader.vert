uniform vec4 position; 	// from 
varying vec4 color;		// to fragment
void main()
{
	//gl_Position = position;
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = ftransform();
	//gl_FrontColor = gl_Color;
}