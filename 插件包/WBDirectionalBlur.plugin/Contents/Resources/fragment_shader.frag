#version 330
uniform sampler2D videoTexture;
uniform float property_float_0;
uniform float property_float_1;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    int Samples = 64; //multiple of 2
    
    vec2 UV = out_uvs.xy;
    
    vec4 Color = vec4(0.0);
    
    float Intensity = 0.2 * property_float_0;
        
    vec2  Direction = vec2(1.0,0.0); //方向向量  此方向为水平
    
    float X = cos(property_float_1)*Direction.x - sin(property_float_1)*Direction.y;
    
    float Y = sin(property_float_1)*Direction.x +cos(property_float_1)*Direction.y;
    
    Direction = vec2(X,Y);
    
    for (int i=1; i<=Samples/2; i++)
    {
    
        Color += texture(videoTexture,UV+float(i)*Intensity/float(Samples/2)*Direction);
    
        Color += texture(videoTexture,UV-float(i)*Intensity/float(Samples/2)*Direction);
    }

    colourOut = Color / float(Samples);
    
}
