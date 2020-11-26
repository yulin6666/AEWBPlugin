
precision highp float;
uniform sampler2D inputImageTexture;
uniform float property_float_0; // slider
uniform int property_int_0; // direction
varying highp vec2 textureCoordinate;


float Gaussian (float sigma, float x)
{
    return exp(-(x*x) / (2.0 * sigma*sigma));
}

void main()
{
    vec2 UV = textureCoordinate.xy;
    int   c_samplesX     = int(25.0 * property_float_0 / 100.0) + 3;
    int   c_samplesY     = int(25.0 * property_float_0 / 100.0) + 3;
    float c_textureSize  = 512.0;
    int   c_halfSamplesX = c_samplesX;
    int   c_halfSamplesY = c_samplesY;
    float c_pixelSize    = (1.0 / c_textureSize) * (1.0 + 2.0 * property_float_0 / 100.0);
    float c_sigmaX       = 20.0;
    float c_sigmaY       = 20.0;

    if (property_int_0 == 2){ //水平
        c_sigmaY = 0.01;
    }else if (property_int_0 == 3){//竖直
        c_sigmaX = 0.01;
    }
    float total = 0.0;
    vec4 ret = vec4(0.0);

    for (int iy = 0; iy < c_samplesY; ++iy)
    {
        float fy = Gaussian (c_sigmaY, float(iy) - float(c_halfSamplesY));
        float offsety = float(iy-c_halfSamplesY) * c_pixelSize;
        for (int ix = 0; ix < c_samplesX; ++ix)
        {
            float fx = Gaussian (c_sigmaX, float(ix) - float(c_halfSamplesX));
            float offsetx = float(ix-c_halfSamplesX) * c_pixelSize;
            total += fx * fy;
            ret += texture2D(inputImageTexture, UV + vec2(offsetx, offsety)) * fx*fy;
        }
    }
    gl_FragColor = ret / total;
    if (property_float_0 == 0.0){
        gl_FragColor = texture2D(inputImageTexture,UV);
    }
}

       
