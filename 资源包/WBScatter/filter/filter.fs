precision highp float;
uniform sampler2D inputImageTexture;
varying highp vec2 textureCoordinate;

uniform float width;
uniform float height;

uniform float uAmount;
uniform float uRandom;
uniform int uType;

float noise(vec2 uv) {
    vec2 seed = vec2(sin(uv.x), cos(uv.y));
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453 * uRandom);
}

//***************************************************************************************************//
void main() {
    vec2 uv = textureCoordinate.xy;
    
    float mCount = uAmount / 6.5;
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
    
    gl_FragColor = texture2D(inputImageTexture, uv);
}
