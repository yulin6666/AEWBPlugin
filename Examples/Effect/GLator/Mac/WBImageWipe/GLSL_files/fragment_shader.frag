#version 330
uniform sampler2D videoTexture;
uniform int count;
uniform int property;//通道
uniform int inverseGradient;//逆转
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
void main( void )
{
    vec2 uv = out_uvs.xy;
    vec4 AEColor = texture(videoTexture,uv);
    vec4 color = vec4(AEColor.yzw,AEColor.x);
    vec3 hsvColor = rgb2hsv(color.xyz);
    float a = 1.0;
    float var = color.r;
    if (property == 2){
        var = color.g;
    }else if(property == 3){
        var = color.b;
    }else if(property == 4){
        var = color.a;
    }else if(property == 5){//Luminance 亮度  hsv中的v
        var = hsvColor.b;
    }else if(property == 6){//hue
        var = hsvColor.r;
    }else if(property == 7){//stauration
        var = hsvColor.g;
    }else if(property == 8){//还是明亮度
        var = hsvColor.b;
    }
    bool decide = inverseGradient == 0 ? (var < float(count) / 100.0 + 0.01 ) : (var > float(count) / 100.0 + 0.01 );
    if (decide){
        a = 0.0;
    }
    color.a = a;
    colourOut = vec4(color.w,color.xyz);
}


