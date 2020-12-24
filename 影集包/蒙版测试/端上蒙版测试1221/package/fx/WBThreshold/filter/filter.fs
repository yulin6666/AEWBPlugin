precision highp float;
uniform sampler2D inputImageTexture;
uniform int count;
varying highp vec2 textureCoordinate;

void main( )
{
    vec2 uv = textureCoordinate.xy;
    float Soft = 0.001;
    float Threshold = 1.0 * float(count) / 100.0;

    float f = Soft/2.0;
    float a = Threshold - f;
    float b = Threshold + f;
    vec4 tx = texture2D(inputImageTexture,uv);
    float l = (tx.x + tx.y + tx.z) / 3.0;
    //smoothstepj函数  l小于a返回a大于b返回b,中间则返回一条平滑曲线
    float v = smoothstep(a, b, l);
    gl_FragColor = vec4(v);
}


