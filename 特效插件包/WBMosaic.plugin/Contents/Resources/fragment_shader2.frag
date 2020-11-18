#version 330
uniform sampler2D videoTexture;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

void main( void )
{
    colourOut = texture( videoTexture, out_uvs.xy );
    colourOut = colourOut;
}
