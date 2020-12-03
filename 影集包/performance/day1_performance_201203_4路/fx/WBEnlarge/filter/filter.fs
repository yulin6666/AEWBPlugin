precision highp float;
uniform sampler2D inputImageTexture;
varying highp vec2 textureCoordinate;

uniform float circle_center_x; //放大镜中心
uniform float circle_center_y; //放大镜中心
uniform int circle_radius;//放大镜圆半径
uniform int zoom_times; //放大倍数
uniform float texelWidth; //纹理宽度
uniform float texelHeight; //纹理高度

vec2 transForTexPosition(vec2 pos){
    return vec2(float(pos.x*texelWidth),float(pos.y*texelHeight));
}

float getDistance(vec2 pos_src,vec2 pos_dist){
    float sum = pow((pos_src.x- pos_dist.x),2.0) + pow((pos_src.y - pos_dist.y),2.0);
    return sqrt(sum);
}

vec2 getZoomPosition(){
    float zoom_x = float(gl_FragCoord.x - circle_center_x)/float(zoom_times);
    float zoom_y = float(gl_FragCoord.y - circle_center_y)/float(zoom_times);
    return vec2(float(circle_center_x+zoom_x),float(circle_center_y+zoom_y));
}

vec4 getColor(){
    vec2 pos = getZoomPosition();
    float _x = floor(pos.x);
    float _y = floor(pos.y);
    float u = pos.x - _x;
    float v = pos.y - _y;
    
    vec4 data_00 = texture2D(inputImageTexture,transForTexPosition(vec2(_x,_y)));
    vec4 data_01 = texture2D(inputImageTexture,transForTexPosition(vec2(_x,_y +1.0)));
    vec4 data_10 = texture2D(inputImageTexture,transForTexPosition(vec2(_x+1.0,_y)));
    vec4 data_11 = texture2D(inputImageTexture,transForTexPosition(vec2(_x+1.0,_y +1.0)));

    return (1.0 - u)*(1.0-v)*data_00+(1.0 - u)*v*data_01+ u*(1.0-v)*data_10 +u*v*data_11;
}

void main()
{
    vec2 frag_pos = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec2 circle_center = vec2(circle_center_x,circle_center_y);
    if(getDistance(circle_center,frag_pos) > float(circle_radius)){//圈外
        gl_FragColor = texture2D(inputImageTexture,textureCoordinate);
    }
    else{//圈内
        gl_FragColor = getColor();
    }
    
}
