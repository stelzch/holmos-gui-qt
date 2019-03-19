varying vec2 v_texCoord;
uniform sampler2D texture;

void main(void)
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.5);
    color.y = mod(v_texCoord.y, 1.0);
    color.x = mod(v_texCoord.x, 1.0);
    //gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);ä
    gl_FragColor = texture2D(texture, v_texCoord);
}
