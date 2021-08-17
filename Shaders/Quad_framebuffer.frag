#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D tex;
uniform int renderMode = 0;
uniform float far = 100.0;
uniform int numShadowCascades = 3;
uniform float shadowSplitDepths[6];
//0 is default, 1 is position, 2 is normal, 3 is albedoSpec, 4 is depth buffer, 5 is specularity, 6 is shadow
//7 is shadow cascades, 9 is SSAO

void main() {
	vec4 color = texture(tex, texCoord);
	if (renderMode == 0) {
		FragColor = color;
	} else if (renderMode == 1) {
		FragColor = normalize(abs(color));
	} else if (renderMode == 2) {
		FragColor = normalize(abs(color));
	} else if (renderMode == 3) {
		FragColor = vec4(color.rgb, 1.0);
	} else if (renderMode == 4) {
		FragColor = vec4(vec3(color.r) / far,1.0);
	} else if (renderMode == 5) {
		FragColor = vec4(color.rgb,1.0);
	} else if (renderMode == 6) {
		FragColor = vec4(vec3(color.a),1.0);
	} else if (renderMode == 7) {
		float depth = color.r;
		vec4 FinalColor = vec4(0.0,0.0,0.0,1.0);
		for (int i = 0; i < numShadowCascades; i++) {
			int index = i % 3;
			if (depth > shadowSplitDepths[i] && depth <= shadowSplitDepths[i+1]) {
				if (index == 0) FinalColor = vec4(1.0,0.0,0.0,1.0);
				if (index == 1) FinalColor = vec4(0.0,1.0,0.0,1.0);
				if (index == 2) FinalColor = vec4(0.0,0.0,1.0,1.0);
			}
		}
		FragColor = FinalColor;
	} else if (renderMode == 9) {
		FragColor = vec4(vec3(color.r),1.0);
	}
	else {
		FragColor = color;
	}
}