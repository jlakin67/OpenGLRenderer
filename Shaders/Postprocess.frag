#version 430 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D screen;
uniform float exposure = 1.0f;

float luminance(vec3 v)
{
    return dot(v, vec3(0.2126f, 0.7152f, 0.0722f));
}

//https://www.shadertoy.com/view/4dBcD1 trying out this tone map operator
vec3 reinhard_jodie(vec3 v, float exposure)
{
    float l = luminance(v);
    vec3 tv = v / (exposure + v);
    return mix(v / (exposure + l), tv, tv);
}

void main() {
    vec4 screenColor = texture(screen, texCoord);
	FragColor = vec4(reinhard_jodie(screenColor.rgb, exposure), 1.0f);
}