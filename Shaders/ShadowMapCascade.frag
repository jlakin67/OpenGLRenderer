#version 430 core

uniform bool containsDiffuse = false;
uniform sampler2D texture_diffuse0;

in GS_FS_Interface {
    vec2 texCoord;
} fs_in;

void main() {
    if (containsDiffuse) {
		if (texture(texture_diffuse0, fs_in.texCoord).a < 0.01) discard;
	}
}