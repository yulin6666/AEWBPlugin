precision highp float;
uniform sampler2D inputImageTexture;
uniform int mirror;
uniform float scaleX;
uniform float scaleY;

varying highp vec2 textureCoordinate;

void main()
{
    float tileX = scaleX;
    float tileY = scaleY;
    vec2 uv = vec2(textureCoordinate.x,textureCoordinate.y) - 0.5;
    
    int rowIndex = int((abs(0.5 - textureCoordinate.x) - (tileX / 2.)) / tileX) + 1;
    if (abs(0.5 - textureCoordinate.x ) <  tileX / 2.){
        rowIndex = 0;
    }
    float rowOddEven = mod(float(rowIndex), 2.);
    
    if (rowOddEven == 1. && mirror == 1){

        uv.x = 1.0 - mod(uv.x,scaleX) * (1. / scaleX);
        
    }else{
        
        uv.x = mod(uv.x,scaleX) * (1. / scaleX);
    }
    
    int columeIndex = int((abs(0.5 - textureCoordinate.y) - (tileY / 2.)) / tileY) + 1;
    if (abs(0.5 - textureCoordinate.t ) <  tileY / 2.){
        columeIndex = 0;
    }
    float columeOddEven = mod(float(columeIndex), 2.);
    
    if (columeOddEven == 1. && mirror == 1) {
        
        uv.y = 1.0 - mod(uv.y,scaleY) * (1. / scaleY);
        
    }else{
        
        uv.y = mod(uv.y,scaleY) * (1. / scaleY);
        
    }
    
    gl_FragColor = texture2D(inputImageTexture,fract(uv + 0.5));
    
}
