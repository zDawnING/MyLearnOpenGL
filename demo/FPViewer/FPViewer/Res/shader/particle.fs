#version 120 // 点精灵的绘制模式下，只有当GLSL的版本大于120版本才能有对应的属性
#ifdef GL_ES

precision mediump float;

#endif

uniform sampler2D U_Texture;

varying vec4 V_Color;

void main(){
    // 点精灵颜色 = 纹理颜色 * 顶点本身颜色，纹理坐标则从gl_PointCoord.xy中取
    gl_FragColor = texture2D(U_Texture, gl_PointCoord.xy) * V_Color;
}
