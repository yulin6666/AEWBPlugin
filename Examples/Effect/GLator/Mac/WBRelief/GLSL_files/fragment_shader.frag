#version 330
uniform sampler2D inputImageTexture;
uniform float intensity;

in vec2 textureCoordinate;
out vec4 colourOut;

in vec2 leftTextureCoordinate;
in vec2 rightTextureCoordinate;

in vec2 topTextureCoordinate;
in vec2 topLeftTextureCoordinate;
in vec2 topRightTextureCoordinate;

in vec2 bottomTextureCoordinate;
in vec2 bottomLeftTextureCoordinate;
in vec2 bottomRightTextureCoordinate;


void main( void )
{
    mat3 convolutionMatrix;
    convolutionMatrix[0][0] = intensity * (-2.0);
    convolutionMatrix[0][1] = intensity * (-1.0);
    convolutionMatrix[0][2] = 0.0;
    
    convolutionMatrix[1][0] = intensity * (-1.0);
    convolutionMatrix[1][1] = 1.0;
    convolutionMatrix[1][2] = intensity;
    
    convolutionMatrix[2][0] = 0.0;
    convolutionMatrix[2][1] = intensity;
    convolutionMatrix[2][2] = intensity*(2.0);
    
    
    mediump vec3 bottomColor = texture(inputImageTexture, bottomTextureCoordinate).gba;
    mediump vec3 bottomLeftColor = texture(inputImageTexture, bottomLeftTextureCoordinate).gba;
    mediump vec3 bottomRightColor = texture(inputImageTexture, bottomRightTextureCoordinate).gba;
    mediump vec4 centerColor = texture(inputImageTexture, textureCoordinate);
    mediump vec3 leftColor = texture(inputImageTexture, leftTextureCoordinate).gba;
    mediump vec3 rightColor = texture(inputImageTexture, rightTextureCoordinate).gba;
    mediump vec3 topColor = texture(inputImageTexture, topTextureCoordinate).gba;
    mediump vec3 topRightColor = texture(inputImageTexture, topRightTextureCoordinate).gba;
    mediump vec3 topLeftColor = texture(inputImageTexture, topLeftTextureCoordinate).gba;

    mediump vec3 resultColor = topLeftColor * convolutionMatrix[0][0] + topColor * convolutionMatrix[0][1] + topRightColor * convolutionMatrix[0][2];
    resultColor += leftColor * convolutionMatrix[1][0] + centerColor.gba * convolutionMatrix[1][1] + rightColor * convolutionMatrix[1][2];
    resultColor += bottomLeftColor * convolutionMatrix[2][0] + bottomColor * convolutionMatrix[2][1] + bottomRightColor * convolutionMatrix[2][2];

    colourOut = vec4(centerColor.r,resultColor);
}

