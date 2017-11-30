#ifdef GL_ES

precision mediump float;

#endif

uniform sampler2D U_Texture;

uniform vec4 U_LightPos; // 光源位置（方向光）
uniform vec4 U_LightAmbient; // 环境光分量
uniform vec4 U_LightDiffuse; // 漫反射光分量
uniform vec4 U_LightSpecular; // 镜面反射光分量

uniform vec4 U_AmbientMaterial; // 环境光材质反射系数
uniform vec4 U_DiffuseMaterial; // 漫反射光材质反射系数
uniform vec4 U_SpecularMaterial; // 镜面反射光材质反射系数

uniform vec4 U_CameraPos; // 相机位置，镜面反射是根据视点来计算的，从不同角度看物体，物体的反光点是不一样的
uniform vec4 U_LightOpt; // 为了镜面反射光更加好看而使用的系数 / w分量作为控制是否纹理带高光效果

varying vec4 V_Color;

varying vec4 V_Normal;

varying vec4 V_WorldPos;

varying vec4 V_Texcoord;

/**
 * 给模型追加一个点光源
 */
vec4 getPointLight(){
    float distance = 0.0;
    float constantFactor = 1.0;
    float linearFactor = 0.0;
    float quadircFactor = 0.0;
    
    vec4 ambientColor = U_LightAmbient * U_AmbientMaterial;
    vec3 L = vec3(0.0, 1.0, 0.0) - V_WorldPos.xyz;
    distance = length(L);
    
    float attenuation = 1.0 / constantFactor + linearFactor * distance + quadircFactor * quadircFactor * distance;
    
    L = normalize(L);
    vec3 n = normalize(V_Normal.xyz);
    
    float diffuseIntensity = max(0.0, dot(L, n));
    vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0) * vec4(0.1, 0.4, 0.6, 1.0) * diffuseIntensity * attenuation * 2.0;
    
    return ambientColor + diffuseColor;
}

void main(){
    
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    
    vec4 ambientColor = U_LightAmbient * U_AmbientMaterial;
    
    vec3 lightPos = U_LightPos.xyz;
    vec3 L = lightPos; // 向量由物体指向光源位置
    // 分别单位化L与法向量n
    L = normalize(L);
    vec3 n = normalize(V_Normal.xyz);
    
    // 为了光线强度不要小于0，dot(L, n) 是 |L| * |n| * cos(L,n的夹角)，这就能定义光照强度(夹角越小，光照越强).原因是在方向光射至物体边缘的时候，L与n形成的夹角为90度，dot值为0，处于物体背对光源的位置的n与L的夹角大于90度，dot值为负数，因此使用max让其与0比较，因为物体背部的光照强度也为0.
    float diffuseIntensity = max(0.0, dot(L, n)); // 漫反射光强度
    
    vec4 diffuseColor = U_LightDiffuse * U_DiffuseMaterial * diffuseIntensity; // 漫反射颜色
    
    // 设置镜面反射光为黑色
    vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
    // 只有当漫反射光强度不为0时，物体才可能具备镜面反射光（由上面分析可得）
    if(diffuseIntensity != 0.0){
        // reflect反射光向量
        vec3 reflectDir = normalize(reflect(-L, n));
        // 反射至相机的向量
        vec3 viewDir = normalize(U_CameraPos.xyz - V_WorldPos.xyz);
        // 这里的光照强度取n次幂是为了使光照的强度呈非线性变化并由很强变很弱，从效果上来看就比较真实
        specularColor = U_LightSpecular * U_SpecularMaterial * pow(max(0.0, dot(viewDir, reflectDir)), U_LightOpt.x);
    }
    
    // 纹理和光照颜色混合有很多种，方式自选，下面示例两种：
    if(U_LightOpt.w == 1.0){
        color = ambientColor + diffuseColor * texture2D(U_Texture, V_Texcoord.xy) + specularColor;
    }else{
        color = ambientColor + diffuseColor + getPointLight();
        color = color * texture2D(U_Texture, V_Texcoord.xy);
    }
    
    gl_FragColor = color;
}
