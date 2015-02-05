#version 150

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 modelViewProjectionMatrix;

uniform float thickness;

out vec4 positionVarying;

void main() {
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	
	vec3 up = vec3(0, 0, 1);
	vec3 dir = normalize(p1 - p0);
	vec3 right = normalize(cross(dir, up));
	right *= thickness;
    
    gl_Position = modelViewProjectionMatrix * vec4(p0 - right, 1.0);
    positionVarying = gl_Position;
    EmitVertex();
    
    gl_Position = modelViewProjectionMatrix * vec4(p0 + right, 1.0);
    positionVarying = gl_Position;
    EmitVertex();
    
    gl_Position = modelViewProjectionMatrix * vec4(p1 - right, 1.0);
    positionVarying = gl_Position;
    EmitVertex();
    
    gl_Position = modelViewProjectionMatrix * vec4(p1 + right, 1.0);
    positionVarying = gl_Position;
    EmitVertex();
    
	EndPrimitive();
}