#version 330
uniform sampler2D videoTexture;
uniform int TileWidth;//缩放x 1.0 - 100.0
uniform int TileHeight;//缩放y 1.0 - 100.0
uniform float center_x;//中心X 0.0 - 1.0
uniform float center_y;//中心Y 0.0 - 1.0
uniform int ScreenWidth;//输出x宽度 0.0 - 1.0
uniform int ScreenHeight;//输出y高度 0.0 - 1.0
uniform int RelativeDrift;//相位 0.0 - 100.
uniform int InverseDrift;//逆转相位 0 or 1
uniform float multiplier16bit;
in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;



void main( void )
{
    float row = 1. / (float(TileWidth) / 100.);

    float column = 1. / (float(TileHeight) / 100.);
 
    //中心点偏移
    vec2  uv = out_uvs.xy - vec2(center_x,center_y);
    
    //每一块所占大小
    float tileX = 1.0 / row;
    
    float tileY = 1.0 / column;
    float newRelativeDrift = (mod(float(RelativeDrift) / 65536.0,360)) / 360.0 * 3.14;
//    找出奇偶,进行偏移
    if (InverseDrift == 0 && newRelativeDrift != 0){
        //当前x,是重复第几个画面
        /**
         以中心点为开始,向两边扩散,计算当前uv.x离中心点的距离,求出倍数,计算奇偶.
         abs(center_x - (uv.x + center_x) 后面再+ centerX,是因为上面减掉了中心点
         */
        int rowIndex = int((abs(center_x - (uv.x + center_x)) - (tileX / 2.)) / tileX) + 1;
        if (abs(center_x - (uv.x + center_x)) <  tileX / 2.){
            rowIndex = 0;
        }
        float oddEven = mod(rowIndex, 2);
        if (oddEven == 1.){
            //偏移大小
            uv.y = uv.y + float(newRelativeDrift);
        }
    }else if (InverseDrift == 1 && newRelativeDrift != 0){
        int cloumnIndex = int((abs(center_y - (uv.y + center_y)) - (tileY / 2.)) / tileY) + 1;
        if (abs(center_y - (uv.y + center_y)) <  tileY / 2.){
            cloumnIndex = 0;
        }
        float oddEven = mod(cloumnIndex, 2);
        if (oddEven == 1.){
            //偏移大小
            uv.x = uv.x + float(newRelativeDrift);
        }
    }

    uv.x = mod((uv.x),tileX) * row;
    
    uv.y = mod((uv.y),tileY) * column;

    colourOut = texture(videoTexture, fract(uv + vec2(center_x,center_y)));
    
    highp float xProgress = ( float(ScreenWidth) / 100.0 / 2.0);
    
    highp float yProgress = ( float(ScreenHeight) / 100.0 / 2.0);
    
    if (xProgress < abs(0.5 - out_uvs.x) || yProgress < abs(0.5 - out_uvs.y)){
        
        colourOut = vec4(0.0,0.0,0.0,1.0);
    }
    
}


