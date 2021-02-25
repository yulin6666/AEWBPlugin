#version 330
uniform sampler2D videoTexture;

in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

uniform float width;
uniform float height;

uniform float uAmount;
uniform float uRandom;
uniform int uType;

/*******************************************************************/

float noise(vec2 uv) {
    vec2 seed = vec2(sin(uv.x), cos(uv.y));
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453 * uRandom);
}

void main(void) {
    vec2 uv = out_uvs.xy;

    float mCount = uAmount / 7.5;
    float radius = abs(mCount);

    float offsetX = radius * noise(uv) * 2.0 - radius;      // 水平
    float offsetY = radius * noise(uv.yx) * 2.0 - radius;   // 垂直

    offsetX /= width;
    offsetY /= height;

    if(uType == 1) {
        uv.x += offsetX;
        uv.y += offsetY;
    } else if(uType == 2) {
        uv.x += offsetX;
    } else {
        uv.y += offsetY;
    }
    
    colourOut = texture(videoTexture, uv);
}
