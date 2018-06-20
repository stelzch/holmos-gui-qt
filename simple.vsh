attribute highp vec4 posAttr;
attribute vec2 texCoordAttr;
uniform float heightScaleAttr;
uniform highp mat4 matrix;

uniform sampler2D texture;

varying vec2 v_texCoord;


void main() {
// posAttr.z = sin(posAttr.x) * 0.1;

    vec4 pos = posAttr.xyzw;
    pos.z += heightScaleAttr * texture2D(texture, texCoordAttr).x;
    gl_Position = matrix * pos;

    v_texCoord = texCoordAttr;
}
