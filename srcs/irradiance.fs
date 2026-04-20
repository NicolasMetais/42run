#version 330
in vec3 localPos;
out vec4 FragColor;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main() {
	vec3 N = normalize(localPos);
	vec3 irradiance = vec3(0.0);

	//Je dessine des abscisses et des ordonnees dans ma cubemap pour localiser ma lumiere.
	vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	vec3 right = normalize(cross(up, N));  //X
	up = normalize(cross(N, right)); //Y

	//phi = rotation autour de N
	//theta = s'eloigne de l'horizon
	float sampleDelta = 0.025;
	float nrSample = 0.0;

	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
			irradiance += pow(texture(environmentMap, sampleVec).rgb, vec3(2.2)) * cos(theta) * sin(theta);
			nrSample++;
		}
	}
	irradiance = PI * irradiance / float(nrSample);
	FragColor = vec4(irradiance, 1.0);
}