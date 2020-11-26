#version 330
uniform sampler2D inputImageTexture;
uniform int HorizontalBlock;
uniform int VerticalBlock;
in vec2 out_uvs;
out vec4 colourOut;

vec2 getUvMapPos(){
    float block_w = 1.0/float(HorizontalBlock);
    float block_x_idx = floor(out_uvs.x/block_w);
    
    float block_h = 1.0/float(VerticalBlock);
    float block_y_idx = floor(out_uvs.y/block_h);
    
    return vec2(block_w * (block_x_idx + 0.5),block_h*(block_y_idx +0.5));
}

void main( void )
{
    highp vec4 originalColor = texture(inputImageTexture, out_uvs.xy);
    vec4 o = vec4(1.0, 1.0, 1.0, 1.0);
    vec2 realPos = getUvMapPos();
    o *= texture(inputImageTexture, realPos);
    colourOut = o;
}

