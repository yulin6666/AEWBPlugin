precision highp float;

attribute vec4 position;
attribute vec2 inputTextureCoordinate;

varying vec2 textureCoordinate;
uniform mat4 modelMatrix;
void main(void)
{
    gl_Position = modelMatrix * position;
    textureCoordinate = inputTextureCoordinate.st; //4个顶点
}
