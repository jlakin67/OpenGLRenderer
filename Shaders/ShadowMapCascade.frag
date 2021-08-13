#version 430 core

uniform bool containsDiffuse = false;
uniform sampler2D texture_diffuse0;
uniform bool containsAlpha = false;
uniform sampler2D texture_alpha;
uniform vec4 color = vec4(0.2f, 0.2f, 0.2f, 1.0f);

in GS_FS_Interface {
    vec2 texCoord;
} fs_in;

void main() {
	float alpha;
	if (containsDiffuse) {
		alpha = texture(texture_diffuse0, fs_in.texCoord).a;
	} else alpha = color.a;
	if (containsAlpha) {
		alpha = texture(texture_alpha, fs_in.texCoord).r;
	}
	if (alpha < 0.01f) discard;
}