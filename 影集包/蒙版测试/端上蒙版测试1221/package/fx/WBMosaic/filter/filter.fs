precision highp float;
uniform sampler2D inputImageTexture;
uniform int HorizontalBlock;
uniform int VerticalBlock;
varying highp vec2 textureCoordinate;

vec2 getUvMapPos(){
    float block_w = 1.0/float(HorizontalBlock);
    float block_x_idx = floor(textureCoordinate.x/block_w);
    
    float block_h = 1.0/float(VerticalBlock);
    float block_y_idx = floor(textureCoordinate.y/block_h);
    
    return vec2(block_w * (block_x_idx + 0.5),block_h*(block_y_idx +0.5));
}


void main()
{
    highp vec4 originalColor = texture2D(inputImageTexture, textureCoordinate.xy);
    vec4 o = vec4(1.0, 1.0, 1.0, 1.0);
    vec2 realPos = getUvMapPos();
    o *= texture2D(inputImageTexture, realPos);
    gl_FragColor = o;
}
