#version 330
uniform sampler2D videoTexture;

uniform float gammaOffset;
uniform float offset;
uniform float exposure;

in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main(void)
{
    vec2 UV = out_uvs.xy;
    vec4 textureColor = texture(videoTexture, UV);
    vec4 rgbaColor = textureColor.gbar;
    vec3 rgbColor = rgbaColor.rgb;
    float gamma = gammaOffset;
    float exp = 1.0f * (exposure+1);
    rgbColor = clamp(exp * rgbColor, 0., 1.);
    rgbColor = pow(rgbColor, vec3(1.0f / gamma-offset));
    rgbaColor.rgb = rgbColor.rgb;
    colourOut = rgbaColor.argb;
}
