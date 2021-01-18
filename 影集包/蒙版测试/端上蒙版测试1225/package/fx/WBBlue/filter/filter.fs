
precision highp float;
varying vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform vec2 u_resolution;
uniform float u_time;

void main()
{
    vec4 color  = texture2D(inputImageTexture, textureCoordinate);
    gl_FragColor = vec4(color.r,color.g,1.0,1.0);
}
