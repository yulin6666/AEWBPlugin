#version 330
uniform sampler2D videoTexture;
uniform float propery_float_0;
uniform float propery_float_1;
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

vec2 getUvMapPos(){
    float block_w = 1.0/propery_float_0;
    float block_x_idx = floor(out_uvs.x/block_w);
    
    float block_h = 1/propery_float_1;
    float block_y_idx = floor(out_uvs.y/block_h);
    
    return vec2(block_w * (block_x_idx + 0.5),block_h*(block_y_idx +0.5));
}

void main( void )
{
    highp vec4 originalColor = texture(videoTexture, out_uvs.xy);
    vec4 o = vec4(1, 1, 1, 1);
    vec2 realPos = getUvMapPos();

    o *= texture(videoTexture, realPos);
    
    colourOut = o;
}

