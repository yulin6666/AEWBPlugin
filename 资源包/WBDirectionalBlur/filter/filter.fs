precision highp float;
uniform sampler2D inputImageTexture;
uniform int length;
uniform int angel;
varying highp vec2 textureCoordinate;

void main()
{
    int Samples = 64; //multiple of 2
    
    vec2 UV = textureCoordinate;
    
    vec4 Color = vec4(0.0);
    
    float Intensity = 0.2 * float(length) / 100.0;
        
    vec2  Direction = vec2(1.0,0.0); //方向向量  此方向为水平
    float newAngel = float(angel) / 360.0 * 3.14;
    float X = cos(newAngel)*Direction.x - sin(newAngel)*Direction.y;
    
    float Y = sin(newAngel)*Direction.x +cos(newAngel)*Direction.y;
    
    Direction = vec2(X,Y);
    
    for (int i=1; i<=Samples/2; i++)
    {
    
        Color += texture2D(inputImageTexture,UV+float(i)*Intensity/float(Samples/2)*Direction);
    
        Color += texture2D(inputImageTexture,UV-float(i)*Intensity/float(Samples/2)*Direction);
    }

    gl_FragColor = Color / float(Samples);
    
}
