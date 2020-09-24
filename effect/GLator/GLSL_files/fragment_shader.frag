#version 330
uniform sampler2D videoTexture;
uniform float sliderVal;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    vec2 uv = out_uvs;
    
    if (uv.x <= 0.5) {
        uv.x = uv.x * 2.0;
    } else {
        uv.x = (uv.x - 0.5) * 2.0;
    }
    if (uv.y <= 0.5) {
        uv.y = uv.y * 2.0;
    } else {
        uv.y = (uv.y - 0.5) * 2.0;
    }
	//simplest texture lookup
	colourOut = texture( videoTexture, fract(uv) ); 

	// in case of 16 bits, convert 32768->65535
	colourOut = colourOut * multiplier16bit;

	// swizzle ARGB to RGBA
	colourOut = vec4(colourOut.g, colourOut.b, colourOut.a, colourOut.r);

	// convert to pre-multiplied alpha
	colourOut = vec4(colourOut.a * colourOut.r, colourOut.a * colourOut.g, colourOut.a * colourOut.b, colourOut.a);

//	// I got the blues
//	colourOut.g = sliderVal;
}
