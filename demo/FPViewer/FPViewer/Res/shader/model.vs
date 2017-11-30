attribute vec4 position;
attribute vec4 color;
attribute vec4 texcoord;
attribute vec4 normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

varying vec4 V_Color;

varying vec4 V_Normal; // 法线

varying vec4 V_WorldPos; // 表示当前点的世界坐标系上的位置

varying vec4 V_Texcoord;

void main(){
    V_Color = color;
    
    V_Normal = normal;
    
    // 世界坐标系上的点
    V_WorldPos = ModelMatrix * position;
    
    V_Texcoord = texcoord;
    
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * position;
}
