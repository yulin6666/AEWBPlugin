#version 330
uniform sampler2D videoTexture;
uniform float property_float_0;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    vec2 uv = out_uvs.xy;
    float Soft = 0.001;
    float Threshold = 1.0 * property_float_0 / 100.0;

    float f = Soft/2.0;
    float a = Threshold - f;
    float b = Threshold + f;
    vec4 tx = texture(videoTexture,uv);
    float l = (tx.x + tx.y + tx.z) / 3.0;
    //smoothstepj函数  l小于a返回a大于b返回b,中间则返回一条平滑曲线
    float v = smoothstep(a, b, l);
    colourOut = vec4(v);
}


