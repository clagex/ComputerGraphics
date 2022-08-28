#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform vec3 uColor;
uniform int uRenderMode;
uniform float uRoughness;
uniform float uDiffRoughness;
uniform float uRefAtNormIncidence;
uniform float uAlbedo;
uniform bool uShowTex;
uniform bool uShowNorm;
uniform sampler2D uTexture;
uniform sampler2D uNormal;

float pi = 3.14159;
vec3 specColor = vec3(0.6);

void cook_torrance(vec3 eye, vec3 lightDir, vec3 halfwayDir, vec3 norm, vec3 surfCol);
void oren_nayar(vec3 eye, vec3 lightDir, vec3 halfwayDir, vec3 norm, vec3 surfCol);

// viewspace data (this must match the output of the fragment shader)
in VertexData {
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
	vec3 lightDir;
	mat3 TBN;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
	vec3 surfCol = uColor;
	if (uShowTex) surfCol = vec3(texture(uTexture, f_in.textureCoord));

	vec3 normal = texture(uNormal, f_in.textureCoord).rgb;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(f_in.TBN * normal);

	// calculate vectors
	vec3 eye = normalize(-f_in.position);
	vec3 lightDir = normalize(f_in.lightDir);
	vec3 halfwayDir = normalize(lightDir + eye);

	vec3 norm = normalize(f_in.normal);

	if (uShowNorm) norm = normal;
	//color = cook_torrance(f_in.position, f_in.normal);

	// output to the frambuffer
	if (uRenderMode == 0) {
		float light = abs(dot(norm, eye));
		vec3 color = mix(surfCol / 4, surfCol, light);
		fb_color = vec4(color, 1);
	}
	else if (uRenderMode == 1) cook_torrance(eye, lightDir, halfwayDir, norm, surfCol);
	else if (uRenderMode == 2) oren_nayar(eye, lightDir, halfwayDir, norm, surfCol);
}

void cook_torrance(vec3 eye, vec3 lightDir, vec3 halfwayDir, vec3 norm, vec3 surfCol) {
	vec3 ambient = 0.2 * surfCol;
	// floats
	float VDotH = clamp(dot(eye, halfwayDir), 0.0, 1.0);
	float NDotH = clamp(dot(norm, halfwayDir), 0.0, 1.0);
	float NDotV = clamp(dot(norm, eye), 0.0, 1.0);
	float NDotL = clamp(dot(norm, lightDir), 0.0, 1.0);
	float r_sq = uRoughness * uRoughness;

	// masking and shadowing
	float geo_numerator = 2.0f * NDotH;
	float geo_denominator = VDotH;

	float geo_b = (geo_numerator * NDotV) / geo_denominator;
	float geo_c = (geo_numerator * NDotL) / geo_denominator;
	float geo = min(1.0f, min(geo_b, geo_c));

	// Beckman

	float roughness_a = 1.0f / (4.0f * r_sq * pow(NDotH, 4));
	float roughness_b = NDotH * NDotH - 1.0f;
	float roughness_c = r_sq * NDotH * NDotH;

	float roughness = roughness_a * exp(roughness_b / roughness_c);

	// Fresnel
	float fresnel = pow(1.0f - VDotH, 5.0f);
	fresnel *= (1.0f - uRefAtNormIncidence);
	fresnel += uRefAtNormIncidence;

	// calculate the specular term in the equation
	vec3 Rs_numerator = vec3(fresnel * geo * roughness);
	float Rs_denominator = NDotV * NDotL;
	vec3 Rs = Rs_numerator / Rs_denominator;

	vec3 final = max(vec3(0.f, 0.f, 0.f), (max(0.0, NDotL) * (specColor * Rs + surfCol))) + ambient;

	// return the result
	fb_color =  vec4(final, 1);
}

void oren_nayar(vec3 eye, vec3 lightDir, vec3 halfwayDir, vec3 norm, vec3 surfCol){
	vec3 ambient = 0.2 * surfCol;
	float LDotV = clamp(dot(lightDir, eye), 0.0, 1.0);
	float NDotL = clamp(dot(norm, lightDir), 0.0, 1.0);
	float NDotV = clamp(dot(norm, eye), 0.0, 1.0);

	float s = LDotV - NDotL * NDotV;
	float t;
	if (s > 0) {
		t = max(NDotL, NDotV);
	} else {
		t = 1.0;
	}

	float denominator = pi + (pi / 2.0f + 2.0f / 3.0f) * uDiffRoughness;
	float A = 1.0f / denominator;
	float B = uDiffRoughness / denominator;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(eye, reflectDir), 0.0), 12);
	vec3 specular = spec * specColor * vec3(1,1,1);

	vec3 final = ambient + uAlbedo * max(0.0, NDotL) * (A + B * s / t) * surfCol + specular;

	fb_color =  vec4(final, 1);
}