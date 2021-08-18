#version 430 core

in vec2 texCoord;
out float FragColor;

uniform sampler2D screen;
uniform int kernelOffset = 4;
uniform float sigma_r = 0.1;
uniform vec2 screenDim = vec2(1600.0f, 900.0f);

float gaussian(float distanceSquared, float sigma) {
	return exp(-distanceSquared / (2.0f*sigma*sigma));
}

void main() {
	float sumWeights = 0.0f;
	float sum = 0.0f;
	float kernelWidth = 2*kernelOffset + 1;
	float sigma_d = kernelWidth/2.0f;
	float centerPixel = texture(screen, texCoord).r;
	for (int i = -kernelOffset; i <= kernelOffset; i++) {
		for (int j = -kernelOffset; j <= kernelOffset; j++) {
			vec2 offset = texCoord + vec2(i,j)/screenDim;
			float samplePixel = texture(screen, offset).r;
			float intensitySquared = (samplePixel - centerPixel)*(samplePixel - centerPixel);
			float weight = gaussian(i*i + j*j, sigma_d)*gaussian(intensitySquared, sigma_r);
			sum += weight*samplePixel;
			sumWeights += weight;
			//sum += samplePixel;
			//sumWeights += 1;
		}
	}
	FragColor = sum/sumWeights;
}