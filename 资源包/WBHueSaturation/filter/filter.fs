
precision highp float;
uniform sampler2D inputImageTexture;

uniform float lightness;
uniform float saturation;
uniform int Hue;

varying highp vec2 textureCoordinate;


float hue2rgb(float f1, float f2, float hue) {
    if (hue < 0.0)
    hue += 1.0;
    else if (hue > 1.0)
    hue -= 1.0;
    float res;
    if ((6.0 * hue) < 1.0)
    res = f1 + (f2 - f1) * 6.0 * hue;
    else if ((2.0 * hue) < 1.0)
    res = f2;
    else if ((3.0 * hue) < 2.0)
    res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
    else
    res = f1;
    return res;
}

vec3 hsl2rgb(vec3 hsl) {
    vec3 rgb;

    if (hsl.y == 0.0) {
        rgb = vec3(hsl.z);// Luminance
    } else {
        float f2;
        if (hsl.z < 0.5)
        f2 = hsl.z * (1.0 + hsl.y);
        else
        f2 = hsl.z + hsl.y - hsl.y * hsl.z;

        float f1 = 2.0 * hsl.z - f2;

        rgb.r = hue2rgb(f1, f2, hsl.x + (1.0/3.0));
        rgb.g = hue2rgb(f1, f2, hsl.x);
        rgb.b = hue2rgb(f1, f2, hsl.x - (1.0/3.0));
    }
    return rgb;
}


void main()
{
    vec2 UV = textureCoordinate.xy;
    
    vec4 textureColor = texture(videoTexture, UV);
    vec4 rgbaColor = textureColor.xyzw;
    vec3 rgbColor = textureColor.rgb;
  
    
        vec3 hsvColor = rgb2hsl(rgbColor);
        float hueOffset = radians(Hue);
        float lumOffset = lightness / 100.0f;
        float satOffset = saturation /100.0f;
        float hsvDeg = asin(hsvColor.x);
        hsvColor.x =  abs(sin(hsvDeg + hueOffset));
        hsvColor.y = hsvColor.y + satOffset;
        hsvColor.z = hsvColor.z + lumOffset;
        hsvColor = clamp(hsvColor, 0., 1.);
        rgbColor = hsl2rgb(hsvColor);
    
    rgbaColor.rgb = rgbColor.rgb;
    gl_FragColor = rgbaColor.rgba;
    
}

       
