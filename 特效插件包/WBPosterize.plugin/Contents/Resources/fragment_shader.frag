#version 330
uniform sampler2D videoTexture;
uniform int count;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    vec2 uv = out_uvs;
    vec4 AEColor = texture(videoTexture,uv);
    vec4 col = vec4(AEColor.yzw,AEColor.x);
    float colorQuality = 2. + 30. * float(count) / 100.0;
    vec3 q = vec3(colorQuality);
    col = vec4(floor(col.rgb*q)/(q - 1.),col.a); // -1是为了还原AE效果
    colourOut = vec4(col.w,col.xyz);
}


