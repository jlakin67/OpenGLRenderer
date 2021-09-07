#version 430 core

out float FragColor;
in vec2 texCoord;

uniform sampler2D upperLeft;
uniform sampler2D upperRight;
uniform sampler2D lowerLeft;
uniform sampler2D lowerRight;

uniform vec2 screenDim = vec2(1600, 900);

void main() {
	ivec2 FragID = ivec2(gl_FragCoord.xy) * ivec2(screenDim);
	ivec2 texOffset = ivec2(mod(FragID, ivec2(2,2)));
	texOffset = 2*texOffset - 1;
	float outColor = 0.0f;
	if (texOffset[0] == 0) {
		if (texOffset[1] == 0) {
			outColor = texture(lowerLeft, texCoord).r;
		} else {
			outColor = texture(upperLeft, texCoord).r;
		}
	} else {
		if (texOffset[1] == 0) {
			outColor = texture(lowerRight, texCoord).r;
		} else {
			outColor = texture(upperRight, texCoord).r;
		}
	}
	FragColor = outColor;
}