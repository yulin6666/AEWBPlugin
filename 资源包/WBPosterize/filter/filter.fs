precision highp float;
uniform sampler2D inputImageTexture;
uniform int count;
varying highp vec2 textureCoordinate;

void main( )
{
    vec2 uv = textureCoordinate.xy;
    vec4 col = texture2D(inputImageTexture,uv);
    float colorQuality = 2. + 30. * float(count) / 100.0;
    vec3 q = vec3(colorQuality);
    col = vec4(floor(col.rgb*q)/(q - 1.),col.a); // -1是为了还原AE效果
    gl_FragColor = col;
}


