#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uLightDirection;
uniform vec3 uSpecColor;
uniform float uShiniess;
uniform sampler2D uTexture;
uniform bool uShowTex;

// viewspace data (this must match the output of the fragment shader)
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
	vec3 color;
} f_in;

// framebuffer output
out vec4 fb_color;

const vec3 lightColor = vec3(1, 1, 1);
void main() {
	// calculate lighting (hack)
	vec3 surfaceColor = f_in.color;
	if (uShowTex) {
		surfaceColor = vec3(texture(uTexture, f_in.textureCoord));
	}
	vec3 eye = normalize(-f_in.position); // direction towards the eye
	vec3 norm = normalize(f_in.normal); 
	vec3 lightDir = normalize(-uLightDirection);
	vec3 ambient = surfaceColor * vec3(0.2, 0.2, 0.2); // k_a * i_a
	vec3 diffuse = max(dot(norm, lightDir), 0.0) * lightColor * surfaceColor; // dot(n,l)*i_d*k_d
	vec3 reflectDir = reflect(-lightDir, norm); // r
	float spec = pow(max(dot(eye, reflectDir), 0.0), uShiniess); // dot(r, v)^m_shine
	vec3 specular = spec * uSpecColor * vec3(1, 1, 1); // spec * i_s * k_s

	vec3 finalColor = ambient + diffuse + specular;

	// output to the frambuffer
	fb_color = vec4(finalColor, 1);

}