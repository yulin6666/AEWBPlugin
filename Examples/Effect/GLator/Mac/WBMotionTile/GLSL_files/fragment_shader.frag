#version 330
uniform sampler2D videoTexture;
uniform float property_float_0;//缩放x 1.0 - 100.0
uniform float property_float_1;//缩放y 1.0 - 100.0
uniform float property_float_2;//中心X 0.0 - 1.0
uniform float property_float_3;//中心Y 0.0 - 1.0
uniform float property_float_4;//输出x宽度 0.0 - 1.0
uniform float property_float_5;//输出y高度 0.0 - 1.0
uniform float property_float_6;//相位 0.0 - 100.
uniform int property_int_0;//逆转相位 0 or 1
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;



void main( void )
{
    float row = 1. / (property_float_0 / 100.);

    float column = 1. / (property_float_1 / 100.);
 
    //中心点偏移
    vec2  uv = out_uvs.xy - vec2(property_float_2,property_float_3);
    
    //每一块所占大小
    float tileX = 1.0 / row;
    
    float tileY = 1.0 / column;
 
    
    //当前x,是重复第几个画面
    float asd = mod(property_float_2 - (tileX/2.0),tileX);
    int rowIndex = int(floor((uv.x - asd + property_float_2) / tileX));
    
    int columnIndex = int((uv.y) / tileY);
    
//    找出奇偶,进行偏移
    if (property_int_0 == 0){

        float oddEven = mod(rowIndex, 2);
        
        float a = mod(floor(row), 2);
        
        if (a == 0.){
            
            a = 1.;
        }else{
            
            a = 0.;
        }
        if (oddEven == a){

            //偏移大小
            uv.y = fract(uv.y + property_float_6);

        }

    }else{

        float oddEven = mod(columnIndex, 2);

        if (oddEven == 0.){

            uv.x = fract(uv.x + property_float_6);

        }
    }
    
    uv.x = mod((uv.x),tileX) * row;
    
    uv.y = mod((uv.y),tileY) * column;

    colourOut = texture(videoTexture, fract(uv + vec2(property_float_2,property_float_3)));
    
    highp float xProgress = ( property_float_4 / 100.0 / 2.0);
    
    highp float yProgress = ( property_float_5 / 100.0 / 2.0);
    
    if (xProgress < abs(0.5 - out_uvs.x) || yProgress < abs(0.5 - out_uvs.y)){
        
        colourOut = vec4(0.0,0.0,0.0,1.0);
    }
    
}


