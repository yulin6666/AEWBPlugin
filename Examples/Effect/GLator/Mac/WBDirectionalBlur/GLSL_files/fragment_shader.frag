#version 330
uniform sampler2D videoTexture;
uniform float sliderVal;
uniform float angel;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    int Samples = 64; //multiple of 2
    
    vec2 UV = out_uvs.xy;
    
    vec4 Color = vec4(0.0);
    
    float Intensity = 0.2 * sliderVal;
        
    vec2  Direction = vec2(1.0,0.0);
    
    float X = cos(angel)*Direction.x - sin(angel)*Direction.y;
    
    float Y = sin(angel)*Direction.x +cos(angel)*Direction.y;
    
    Direction = vec2(X,Y);
    
    for (int i=1; i<=Samples/2; i++)
    {
    
        Color += texture(videoTexture,UV+float(i)*Intensity/float(Samples/2)*Direction);
    
        Color += texture(videoTexture,UV-float(i)*Intensity/float(Samples/2)*Direction);
    }

    colourOut = Color / float(Samples);
    
}
