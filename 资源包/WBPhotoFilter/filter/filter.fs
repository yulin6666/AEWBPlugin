precision highp float;
uniform sampler2D inputImageTexture;
uniform int density;
uniform int ColorPop;
uniform float palette_r;
uniform float palette_g;
uniform float palette_b;
uniform float palette_a;
uniform int keepluminosity;
varying highp vec2 textureCoordinate;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main( )
{
    vec2 uv = textureCoordinate.xy;
    vec4 filterColor = vec4(0.0,0.0,0.0,0.0);
    if (ColorPop == 1) {//红
        filterColor = vec4(1.0,0.0,0.0,1.0);
    }else if(ColorPop == 2) {//绿
        filterColor = vec4(0.0,1.0,0.0,1.0);
    }else if(ColorPop == 3) {//蓝
        filterColor = vec4(0.0,0.0,1.0,1.0);
    }else if(ColorPop == 4) {//自定义
        filterColor = vec4(
                           palette_r/255.0,
                           palette_g/255.0,
                           palette_b/255.0,
                           palette_a/255.0);
    }
    
    
    vec4 col = texture2D(inputImageTexture,uv);
    float lum = rgb2hsv(col.xyz).b * (1.0 + float(density) / 2.0);//亮度
    vec4 outColor = mix(col, col * filterColor, float(density) / 200.0 );
    vec3 outLum = outColor.rgb * (1.0 + lum / 50.);
    if (keepluminosity == 0){
        outLum = outColor.rgb;
    }
    gl_FragColor = vec4(outLum,outColor.w);

}



