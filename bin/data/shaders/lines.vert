#version 150

in vec4 position;
in vec2 texcoord;

out vec4 positionVarying;

uniform mat4 modelViewProjectionMatrix;

void main(){
    
	//get our current vertex position so we can modify it
	vec4 pos =  position;
	
	gl_Position = pos;
    positionVarying = pos;
}
