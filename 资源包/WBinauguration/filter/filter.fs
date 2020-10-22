precision highp float;
varying highp vec2 textureCoordinate;
uniform highp float progress;
uniform highp int type;
uniform sampler2D inputImageTexture;
uniform sampler2D inputImageTexture2;

void main()
{
    highp vec4 originalColor = texture2D(inputImageTexture2,textureCoordinate);
    highp float nprogress = (progress / 2.0);
    if (nprogress < abs(0.5 - textureCoordinate.y)){
        originalColor = vec4(0.0,0.0,0.0,1.0);
    }
    gl_FragColor = originalColor;
}
