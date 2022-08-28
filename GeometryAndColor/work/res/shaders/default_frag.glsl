#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uShaderColor;
uniform vec3 uLightDirection;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
	vec3 position;
	vec3 normal;
} f_in;

// framebuffer output
out vec4 fb_color;

const vec3 lightColor = vec3(1, 1, 1);
void main() {
	// calculate shading
	vec3 surfaceColor = uShaderColor;
	vec3 eye = normalize(-f_in.position); // direction towards the eye
	vec3 norm = normalize(f_in.normal); 
	vec3 lightDir = normalize(-uLightDirection);

	vec3 ambient = uShaderColor * vec3(0.4, 0.4, 0.4); // k_a * i_a
	vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor * uShaderColor; // dot(n,l)*i_d*k_d
	vec3 reflectDir = reflect(-lightDir, norm); // r
	float spec = pow(max(dot(eye, reflectDir), 0.0), 16); // dot(r, v)^m_shine
	vec3 specular = spec * lightColor * vec3(1, 1, 1); // spec * i_s * k_s

	vec3 finalColor = ambient + diffuse + specular;

	// output to the frambuffer
	fb_color = vec4(finalColor, 1);
}
