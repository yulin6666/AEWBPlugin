#version 330
uniform sampler2D inputImageTexture;
in vec2 out_uvs;

uniform float center_x; //放大镜中心
uniform float center_y; //放大镜中心
uniform int radius;//放大镜圆半径
uniform int zoom_times; //放大倍数
uniform float texelWidth; //纹理宽度
uniform float texelHeight; //纹理高度
out vec4 colourOut;

vec2 transForTexPosition(vec2 pos){
    return vec2(float(pos.x/texelWidth),float(pos.y/texelHeight));
}

float getDistance(vec2 pos_src,vec2 pos_dist){
    float sum = pow((pos_src.x- pos_dist.x),2.0) + pow((pos_src.y - pos_dist.y),2.0);
    return sqrt(sum);
}

vec2 getZoomPosition(){
    float zoom_x = float(out_uvs.x*texelWidth - center_x*texelWidth)/float(float(zoom_times+20.0)/20.0);
    float zoom_y = float(out_uvs.y*texelHeight - center_y*texelHeight)/float(float(zoom_times+20.0)/20.0);
    return vec2(float(center_x*texelWidth+zoom_x),float(center_y*texelHeight+zoom_y));
}

vec4 getColor(){
    vec2 pos = getZoomPosition();
    float _x = floor(pos.x);
    float _y = floor(pos.y);
    float u = pos.x - _x;
    float v = pos.y - _y;
    
    vec4 data_00 = texture(inputImageTexture,transForTexPosition(vec2(_x,_y)));
    vec4 data_01 = texture(inputImageTexture,transForTexPosition(vec2(_x,_y +1.0)));
    vec4 data_10 = texture(inputImageTexture,transForTexPosition(vec2(_x+1.0,_y)));
    vec4 data_11 = texture(inputImageTexture,transForTexPosition(vec2(_x+1.0,_y +1.0)));

    return (1.0 - u)*(1.0-v)*data_00+(1.0 - u)*v*data_01+ u*(1.0-v)*data_10 +u*v*data_11;
}

void main( void )
{
    vec2 frag_pos = vec2(out_uvs.x*texelWidth, out_uvs.y*texelHeight);
    vec2 center = vec2(center_x*texelWidth,center_y*texelHeight);
    if(getDistance(center,frag_pos) > float(radius)){//圈外
        colourOut = texture(inputImageTexture,out_uvs);
    }
    else{//圈内
        //colourOut =vec4(1.0,1.0,0.0,1.0);
        colourOut =getColor();
    }
}

