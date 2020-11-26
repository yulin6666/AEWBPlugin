#version 330
uniform sampler2D videoTexture;
uniform int progress;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    highp vec4 originalColor = texture(videoTexture, out_uvs.xy);
    highp float nprogress = (float(progress) / 100. / 2.0);
    if (nprogress < abs(0.5 - out_uvs.y)){
        originalColor = vec4(0.0,0.0,0.0,1.0);
    }
    colourOut = originalColor;
}
