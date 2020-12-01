
precision highp float;
uniform sampler2D inputImageTexture;

uniform float gammaOffset;
uniform float offset;
uniform float exposure;

varying highp vec2 textureCoordinate;


void main()
{
    vec2 UV = textureCoordinate.xy;
    
    vec4 textureColor = texture(videoTexture, UV);
    vec4 rgbaColor = textureColor.xyzw;
    vec3 rgbColor = textureColor.rgb;
    float gamma = gammaOffset;
    float exp = 1.0f * (exposure+1);
    rgbColor = clamp(exp * rgbColor, 0.0f, 1.);
    rgbColor = pow(rgbColor, vec3(1.0f / gamma - offset));
    rgbaColor.rgb = rgbColor.rgb;
    gl_FragColor = rgbaColor.rgba;
    
}

       
