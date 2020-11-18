#version 330
in vec4 Position;
in vec2 UVs;
out vec4 out_pos;
out vec2 out_uvs;
uniform mat4 ModelviewProjection;

uniform float texelWidth;
uniform float texelHeight;

out vec2 textureCoordinate;
out vec2 leftTextureCoordinate;
out vec2 rightTextureCoordinate;

out vec2 topTextureCoordinate;
out vec2 topLeftTextureCoordinate;
out vec2 topRightTextureCoordinate;

out vec2 bottomTextureCoordinate;
out vec2 bottomLeftTextureCoordinate;
out vec2 bottomRightTextureCoordinate;

void main( void )
{
    out_pos = ModelviewProjection * Position;
    gl_Position = out_pos;
    
    vec2 widthStep = vec2(texelWidth, 0.0);
    vec2 heightStep = vec2(0.0, texelHeight);
    vec2 widthHeightStep = vec2(texelWidth, texelHeight);
    vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);

    textureCoordinate = UVs.xy;
    leftTextureCoordinate = UVs.xy - widthStep;
    rightTextureCoordinate = UVs.xy + widthStep;

    topTextureCoordinate = UVs.xy - heightStep;
    topLeftTextureCoordinate = UVs.xy - widthHeightStep;
    topRightTextureCoordinate = UVs.xy + widthNegativeHeightStep;

    bottomTextureCoordinate = UVs.xy + heightStep;
    bottomLeftTextureCoordinate = UVs.xy - widthNegativeHeightStep;
    bottomRightTextureCoordinate = UVs.xy + widthHeightStep;
    
	out_uvs = UVs;
}
