#version 330
uniform sampler2D videoTexture;
uniform int length; // slider
uniform float multiplier16bit;
uniform int direction; // direction
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;


float Gaussian (float sigma, float x)
{
    return exp(-(x*x) / (2.0 * sigma*sigma));
}

void main( void )
{
    vec2 UV = out_uvs.xy;
    int   c_samplesX     = int(25 * float(length) / 100.0) + 3;
    int   c_samplesY     = int(25 * float(length) / 100.0) + 3;
    float c_textureSize  = 512.0;
    int   c_halfSamplesX = c_samplesX / 2;
    int   c_halfSamplesY = c_samplesY / 2;
    float c_pixelSize    = (1.0 / c_textureSize) * (1.0 + 2.0 * float(length) / 100.0);
    float c_sigmaX       = 20.0 ;
    float c_sigmaY       = 20.0 ;
    
    if (direction == 2){ //水平
        c_sigmaY = 0.01;
    }else if (direction == 3){//竖直
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
            ret += texture(videoTexture, UV + vec2(offsetx, offsety)) * fx*fy ;
        }
    }
    colourOut = ret / total;
    if (float(length) == 0.0){
        colourOut = texture(videoTexture,UV);
    }
}
