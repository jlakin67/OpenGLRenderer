#version 430 core

out float FragColor;
in vec2 texCoord;

uniform sampler2D upperLeft;
uniform sampler2D upperRight;
uniform sampler2D lowerLeft;
uniform sampler2D lowerRight;

uniform vec2 screenDim = vec2(1600, 900);

void main() {

	ivec2 FragID = ivec2(gl_FragCoord.xy/2.0f);
	ivec2 texOffset = ivec2(mod(FragID, ivec2(2,2)));
	//texOffset = 2*texOffset - 1;
	float outColor = 0.0f;
	if (texOffset[0] == 0) {
		if (texOffset[1] == 0) {
			outColor = texelFetch(lowerLeft, FragID, 0).r;
		} else {
			outColor = texelFetch(upperLeft, FragID, 0).r;
		}
	} else {
		if (texOffset[1] == 0) {
			outColor = texelFetch(lowerRight, FragID, 0).r;
		} else {
			outColor = texelFetch(upperRight, FragID, 0).r;
		}
	}
	FragColor = outColor;
}