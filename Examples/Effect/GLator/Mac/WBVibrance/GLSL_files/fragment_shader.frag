#version 330
uniform sampler2D videoTexture;
uniform float uVibrance;
uniform float uSaturation;

in vec4 out_pos;
in vec2 out_uvs;
out vec4 colourOut;

/*******************************************************************/

mediump vec3 nlRGB2RGB(mediump vec3 nlRGB) {
    mediump vec3 RGB;
    mediump vec3 mask = step(vec3(0.04045),nlRGB);
    RGB = mix((25.0*nlRGB/323.0), pow((200.0*nlRGB +11.0)/211.0,vec3(12.0/5.0)),mask);
    return clamp(RGB,0.0,1.0);
}

mediump vec3 RGB2nlRGB(mediump vec3 RGB) {
    mediump vec3 nlRGB;
    mediump vec3 mask = step(vec3(0.0031308),RGB);
    nlRGB = mix((323.0 * RGB / 25.0),(211.0 * pow(RGB,vec3(5.0/12.0)) - 11.0)/200.0,mask);
    return clamp(nlRGB,0.0,1.0);
}

mediump vec3 adjust_vibrance(vec3 image,float vibrance)
{
    vec3 outCol;

    mediump vec3 delta;
    mediump float boost;
    const mediump float EPSILON = 1e-8;
    
    float dampen_factor;
    float scale_factor;
    if(vibrance < 0.0) {
        dampen_factor = 0.4;  // 0.7
        scale_factor = 1.5;   // 2.0
    } else {
        dampen_factor = 0.7;  // 0.7
        scale_factor = 2.0;   // 2.0
    }
        
    mediump vec3 rgb1 = clamp(image,0.0001,0.9999);
    mediump float r1 = rgb1.x;
    mediump float g1 = rgb1.y;
    mediump float b1 = rgb1.z;
    delta = image - rgb1;
    mediump float gray = (rgb1.x + rgb1.y + rgb1.z)/3.0;
  
    mediump float gi = 1.0 / (gray + EPSILON);
    mediump float gii = 1.0 /(1.0 - gray + EPSILON);
    mediump vec3 sat_rgb = max((rgb1 - gray) *gii,(gray - rgb1) * gi);
    sat_rgb = clamp(sat_rgb,-99999.0,0.99999);
    mediump float rsat = sat_rgb.x;
    mediump float sat = max(max(sat_rgb.x,sat_rgb.y),sat_rgb.z);
    
    mediump float skin = min(r1 - g1,g1 *2.0 - b1) * 4.0 *(1.0 - rsat) * gi;
    skin = clamp(skin,0.0,1.0);
    skin = dampen_factor * skin + 0.15;
    mediump float tsat = 1.0 - pow((1.0 - sat),(1.0 + vibrance));
    boost = (1.0 - skin) * (tsat / (sat + EPSILON) - 1.0);
   
    boost = clamp(boost * scale_factor,-1.0,99999.0);
    
    mediump float gray_ = (image.x + image.y + image.z)/3.0;
    
    
    outCol = clamp(image + (image - gray_) * boost,0.0,1.0);
    outCol = outCol + delta;
 
    return clamp(outCol,0.0,1.0);
}

mediump vec3 adjust_saturation(mediump vec3 image,mediump float saturation) {
    vec3 outCol = nlRGB2RGB(image.rgb);
    mediump float scale_factor = 0.65;
    const mediump vec3 rgb_boost = vec3(1.0, 1.0, 1.0);
    if(saturation < 0.0)
    {
        scale_factor = 1.0;
    }
    mediump float gray = (outCol.x + outCol.y + outCol.z)/3.0;
    mediump vec3 color_diff = outCol - gray;
    mediump float multiplier = max(0.0,1.0 + saturation * scale_factor);
    mediump vec3 color_boost = 1.0 + (multiplier - 1.0) * rgb_boost;
    mediump vec3 img_adj = gray + color_diff * color_boost;
    
    outCol.rgb = RGB2nlRGB(img_adj);
    return clamp(outCol,0.0,1.0);
}

void main(void) {
    vec2 uv = out_uvs.xy;
    vec4 col = texture(videoTexture, uv);
    
    vec4 rgbaColor = col.gbar;
    vec3 rgbColor = rgbaColor.rgb;
    float mVibrance;
    mVibrance = uVibrance * 0.5;

    // 需要做俩次
    rgbaColor.rgb = adjust_vibrance(rgbaColor.rgb, mVibrance);
    rgbaColor.rgb = adjust_vibrance(rgbaColor.rgb, mVibrance);

    rgbaColor.rgb = adjust_saturation(rgbaColor.rgb, uSaturation);
    

    
    colourOut = rgbaColor.argb;
}
