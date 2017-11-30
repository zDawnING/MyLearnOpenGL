attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;
attribute vec4 texcoord;


uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

varying vec4 V_Color;
varying vec4 V_Texcoord;

varying vec3 V_Normal;
varying vec3 V_WorldPos;


// 由于地面的法线是垂直向上的，因此不需要使用IT_Matrix进行处理

void main(){
    V_Color = color;
    V_Texcoord = texcoord;
    V_Normal = normal.xyz;
    V_WorldPos = (ModelMatrix * position).xyz;
    
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * position;
}
