#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uColor;
uniform vec3 uLightPos;

// mesh data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// model data (this must match the input of the vertex shader)
out VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
	vec3 lightDir;
	mat3 TBN;
} v_out;

void main() {
	// transform vertex data to viewspace
	v_out.position = (uModelViewMatrix * vec4(aPosition, 1)).xyz;
	v_out.normal = normalize((uModelViewMatrix * vec4(aNormal, 0)).xyz);
	v_out.textureCoord = aTexCoord;

	vec3 lightP = (vec4(uLightPos, 1)).xyz;
	v_out.lightDir = normalize(lightP - aPosition);

	vec3 tangent = normalize(cross(vec3(0, 0, 1), aNormal));

	vec3 T = normalize(vec3(uModelViewMatrix * vec4(tangent, 0.0f)));
	vec3 N = normalize(vec3(uModelViewMatrix * vec4(aNormal, 0.0f)));
	//T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));

	v_out.TBN = mat3(T, B, N);

	// set the screenspace position (needed for converting to fragment data)
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1);
}