precision highp float;
uniform sampler2D inputImageTexture;
uniform float property_float_0;
uniform float property_float_1;
varying highp vec2 textureCoordinate;

void main()
{
    int Samples = 64; //multiple of 2
    
    vec2 UV = textureCoordinate;
    
    vec4 Color = vec4(0.0);
    
    float Intensity = 0.2 * property_float_0 / 100.0;
        
    vec2  Direction = vec2(1.0,0.0); //方向向量  此方向为水平
    
    float X = cos(property_float_1)*Direction.x - sin(property_float_1 / 65536.0)*Direction.y;
    
    float Y = sin(property_float_1)*Direction.x +cos(property_float_1 / 65536.0)*Direction.y;
    
    Direction = vec2(X,Y);
    
    for (int i=1; i<=Samples/2; i++)
    {
    
        Color += texture2D(inputImageTexture,UV+float(i)*Intensity/float(Samples/2)*Direction);
    
        Color += texture2D(inputImageTexture,UV-float(i)*Intensity/float(Samples/2)*Direction);
    }

    gl_FragColor = Color / float(Samples);
    
}
