#version 330
uniform sampler2D videoTexture;
uniform int density;
uniform int ColorPop;
uniform int palette_r;
uniform int palette_g;
uniform int palette_b;
uniform int palette_a;
uniform int keepluminosity;
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
vec3 hsv2rgb(vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
    
    return c.z * mix( vec3(1.0), rgb, c.y);
}
void main( void )
{
    vec2 uv = out_uvs;
    vec4 filterColor = vec4(0.0,0.0,0.0,0.0);
    
    if (ColorPop == 1) {//红
        filterColor = vec4(1.0,0.0,0.0,1.0);
    }else if(ColorPop == 2) {//绿
        filterColor = vec4(0.0,1.0,0.0,1.0);
    }else if(ColorPop == 3) {//蓝
        filterColor = vec4(0.0,0.0,1.0,1.0);
    }else if(ColorPop == 4) {//自定义
        filterColor = vec4(
                           float(palette_r)/255.0,
                           float(palette_g)/255.0,
                           float(palette_b)/255.0,
                           float(palette_a)/255.0);
    }
    
    
    vec4 AEColor = texture(videoTexture, uv);
    vec4 col = vec4(AEColor.yzw,AEColor.x);
    float lum = rgb2hsv(col.xyz).b * (1.0 + density / 2.0);//亮度
    vec4 outColor = mix(col, col * filterColor, density / 200.0 );
    vec3 outLum = outColor.rgb * (1.0 + lum / 50.);
    if (keepluminosity == 0){
        outLum = outColor.rgb;
    }
    colourOut = vec4(outColor.w,outLum);
}
