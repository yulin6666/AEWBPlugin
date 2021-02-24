precision highp float;
uniform sampler2D inputImageTexture;
uniform int mirror;
uniform float scaleX;
uniform float scaleY;

varying highp vec2 textureCoordinate;

void main()
{

    vec2 uv = vec2(textureCoordinate.x,textureCoordinate.y);
    
    int rowIndex = int(uv.x / scaleX);

    float rowOddEven = mod(float(rowIndex), 2.);
    
    if (rowOddEven == 1. && mirror == 1){

        uv.x = 1. - mod(uv.x,scaleX) * (1. / scaleX);
        
    }else{
        
        uv.x = mod(uv.x,scaleX) * (1. / scaleX);
    }
    
    int columeIndex = int(uv.y / scaleY);
    
    float columeOddEven = mod(float(columeIndex), 2.);
    
    if (columeOddEven == 1. && mirror == 1) {
        
        uv.y = 1. - mod(uv.y,scaleY) * (1. / scaleY);
        
    }else{
        
        uv.y = mod(uv.y,scaleY) * (1. / scaleY);
        
    }
    
    gl_FragColor = texture2D(inputImageTexture,vec2(uv.x,uv.y));
    
}
