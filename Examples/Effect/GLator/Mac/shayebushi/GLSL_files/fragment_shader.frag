#version 330
uniform sampler2D videoTexture;
uniform float sliderVal;
uniform float aaaa;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    
     vec2 p = out_uvs.xy - 0.5;
     float r = sqrt(p.x * p.x + p.y * p.y);
     float a = abs(atan(p.y / p.x));//tan夹角的弧度//因为弧度有正负所以要绝对值
     float tau = 2.0 * 3.1416;//圆一周的弧度
    a = mod(a , tau/sliderVal);//求余
    a = abs(a - tau/sliderVal/2.0);//单位弧度内镜像
    vec2 newTexCoord = r * (vec2(sin(a),cos(a)) + 0.5);
	//simplest texture lookup
    if(sliderVal < 1){
        colourOut = texture( videoTexture, out_uvs.xy);
    }else{
        colourOut = texture( videoTexture, newTexCoord);
    }
    colourOut = vec4(1.0,1.0,1.0,1.0);
	// in case of 16 bits, convert 32768->65535
//	colourOut = colourOut * multiplier16bit;
//	// swizzle ARGB to RGBA
//	colourOut = vec4(colourOut.g, colourOut.b, colourOut.a, colourOut.r);
//	// convert to pre-multiplied alpha
//	colourOut = vec4(colourOut.a * colourOut.r, colourOut.a * colourOut.g, colourOut.a * colourOut.b, colourOut.a);
	// I got the blues
//	colourOut.g = sliderVal;
}
