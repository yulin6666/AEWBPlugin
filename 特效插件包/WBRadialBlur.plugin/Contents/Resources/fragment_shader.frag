#version 330
uniform sampler2D videoTexture;
uniform float property_float_0;
uniform float property_float_1;
uniform float property_float_2;
uniform int property_int_0;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

mat2 rotate2d(float angle) {
    vec2 sc = vec2(sin(angle), cos(angle));
    return mat2( sc.y, -sc.x, sc.x, sc.y );
}

void main( void )
{
    vec2 uv = out_uvs.xy;
    vec2 center = vec2(property_float_1,property_float_2);
    if (property_int_0 == 1){
        float strength = property_float_0 * 50 + 1;
        vec2 dir = center - uv;
        vec2 blurVector = dir * 0.02;
        vec4 accumulateColor = vec4(0.0);
        for (int i = 0; i < int(strength); i++) {
            accumulateColor += texture(videoTexture, uv);
            uv.xy += blurVector.xy;
        }
        vec4 result = accumulateColor/int(strength);
        colourOut = result;
    }else{
        int samples = int(100 * property_float_0);
        float power = 0.001;
        vec4 fragColor = vec4(0.0,0.0,0.0,0.0);
        vec2 m = center;
        vec2 uv2 = out_uvs.xy;
        for (int i = 0; i < samples; i ++)
        {
            uv -= m;
            uv *= rotate2d( power * float(i) );
            uv += m;
            fragColor += pow(texture(videoTexture, uv), vec4(1.));
            uv2 -= m;
            uv2 *= rotate2d( power * float(-i) );
            uv2 += m;
            fragColor += pow(texture(videoTexture, uv2), vec4(1.));
        }
        fragColor /= float(samples * 2);
        colourOut = pow(fragColor, vec4(1./1.));
        if (property_float_0 == 0.0){
            colourOut = texture(videoTexture, uv);
        }
    }
    
}
