#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D U_Texture;

uniform vec4 U_LightPos; // 地面灯光位置
uniform vec4 U_LightAmbient; // 地面环境光分量
uniform vec4 U_LightDiffuse; // 地面漫反射光分量
uniform vec4 U_AmbientMaterial; // 地面环境光反射系数
uniform vec4 U_DiffuseMaterial; // 地面漫反射光反射系数

varying vec4 V_Color;
varying vec4 V_Texcoord;

varying vec3 V_Normal;
varying vec3 V_WorldPos;

void main(){
    
    vec4 color = vec4(0.0, 0.0, 0.0, 0.4);
    
    // 设置点光源会使地面更加明显
    float distance = 0.0; // 受光点与光源的距离
    float constantFactor = 1.0; // 常数衰减因子
    float linearFactor = 0.0; // 线性衰减因子
    float quadricFactor = 0.0; // 平方衰减因子
    
    vec3 L = U_LightPos.xyz - V_WorldPos; //向量由地面坐标指向灯光坐标(即distance对应的向量)
    distance = length(L); // 向量模长
    
    // 根据点光源光照强度衰减公式
    float attenuation = 1.0 / constantFactor + (linearFactor * distance) + (quadricFactor * quadricFactor * distance);
    
    // 环境光颜色
    vec4 ambientColor = U_LightAmbient * U_AmbientMaterial;
    
    L = normalize(L);
    vec3 n = normalize(V_Normal.xyz);
    // 漫反射强度
    float diffuseIntensity = max(0.0, dot(L, n));
    
    if(U_AmbientMaterial.x < 0.5){ // 定义为黑夜
        // 漫反射颜色（初始亮度）* 衰减系数
        vec4 diffuseColor = U_LightDiffuse * U_DiffuseMaterial * diffuseIntensity * attenuation * 3.0;
        color = ambientColor + diffuseColor;
    }else{
        vec4 diffuseColor = U_LightDiffuse * U_DiffuseMaterial;
        color = ambientColor + diffuseColor;
    }
    
    gl_FragColor = color * texture2D(U_Texture, V_Texcoord.xy);
}


