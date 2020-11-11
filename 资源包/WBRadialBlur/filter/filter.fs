precision highp float;
uniform sampler2D inputImageTexture;
uniform float property_float_0;
uniform float property_float_1;
uniform float property_float_2;
uniform int property_int_0;
varying highp vec2 textureCoordinate;


mat2 rotate2d(float angle) {
    vec2 sc = vec2(sin(angle), cos(angle));
    return mat2( sc.y, -sc.x, sc.x, sc.y );
}

void main()
{
    vec2 uv = textureCoordinate.xy;
    vec2 center = vec2(property_float_1 / 65536.0,property_float_2 / 65536.0);
    if (property_int_0 == 1){
        float strength = property_float_0 / 100.0 * 50.0 + 1.0;
        vec2 dir = center - uv;
        vec2 blurVector = dir * 0.02;
        vec4 accumulateColor = vec4(0.0);
        for (int i = 0; i < int(strength); i++) {
            accumulateColor += texture2D(inputImageTexture, uv);
            uv.xy += blurVector.xy;
        }
        vec4 result = accumulateColor/strength;
        gl_FragColor = result;
    }else{
        int samples = int(100.0 * property_float_0 / 100.0);
        float power = 0.001;
        vec4 fragColor = vec4(0.0,0.0,0.0,0.0);
        vec2 m = center;
        vec2 uv2 = textureCoordinate.xy;
        for (int i = 0; i < samples; i ++)
        {
            uv -= m;
            uv *= rotate2d( power * float(i) );
            uv += m;
            fragColor += pow(texture2D(inputImageTexture, uv), vec4(1.));
            uv2 -= m;
            uv2 *= rotate2d( power * float(-i) );
            uv2 += m;
            fragColor += pow(texture2D(inputImageTexture, uv2), vec4(1.));
        }
        fragColor /= float(samples * 2);
        gl_FragColor = pow(fragColor, vec4(1./1.));
        if (property_float_0 == 0.0){
            gl_FragColor = texture2D(inputImageTexture, uv);
        }
    }
}
