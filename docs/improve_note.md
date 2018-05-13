# OpenGL进阶开发流程

在基础流程中，所有的绘图数据和编码都是在CPU上运行的，这种图形程序在较大的数据量中会产生很大的性能消耗，因此在进阶阶段要改变策略，将渲染代码上传至GPU上执行。另外这里仅提供着色器的使用方案，不做太多本质的说明，如果想了解着色器基础的内容请阅读[LearnOpenGL-Shader](https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/), 更多的渲染相关内容请阅读[Real-Time Rendering](http://www.realtimerendering.com)

> 环境：Mac OSX 系统, Xcode
> 
> 使用类库：
>
>`libsoil`:OpenGL图像库,辅助处理各种格式的纹理贴图并生成纹理对象；
>
>`GLM`:OpenGL Mathematics（矩阵库），辅助生成各种向量和矩阵等算法。

## VertexBufferObject的使用（VBO）
从CPU将数据传至GPU后，无法再用C/C++语言进行对上传的数据进行变更，因此我们需要一个从CPU到GPU的通讯手段，就是VBO
示例代码：
``` c++
    GLuint vbo;

    float data[] = {
       -0.2f, -0.2f, 0.0f, 1.0f,
       0.2f, -0.2f, 0.0f, 1.0f,
       0.0f, 0.2f, 0.0f, 1.0f
    };
    
    // 表示新建一个vbo, 传入vbo标识变量地址，opengl的驱动会修改这里的值转变成opengl可识别的标识符，指向显卡中的内存块
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // 把vbo设置成为当前的vbo
    // 在GPU端开辟12个浮点数占用的空间, 设置不再更改显卡上的数据(这里即完成CPU到GPU的传输)
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, data, GL_STATIC_DRAW);
   
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 重置
```
另外，与vbo功能类似的一种对象叫ebo, 不同之处在于ebo使用索引进行点的绘制控制，而vbo则是按列表顺序绘制
示例代码：
``` c++
    GLuint ebo;

    unsigned short indexs[] = { 0, 1, 2 }; // 这里是对应vbo中的点
    glGenBuffers(1, &ebo); // 创建ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*3, indexs, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
```

## Shader的使用

1. 编写shader

    基础的shader有两个：`vertexShader`, `fragmentShader`, 文件资源后缀分别为vs,fs
    
    `vertexShader` 的基本属性类型：
    1. attribute 一般放置位置数据，顶点颜色，纹理坐标，法线数据等;
    2. uniform 一般放置MVP矩阵（ModelMatrix, ViewMatrix, ProjectionMatrix）;
    3. varying 用于与fragmentShader共享数据，这里的值经过顶点着色器后会进行线性插值处理
    
    `fragmentShader` 的基本属性类型：
    1. uniform: 一般放置纹理采样器（type为sampler2D）, 各种用于渲染的向量数据或光照参数等;
    2. varying: 会得到经过线性插值的每个像素点的共享数据

    vs中给`gl_Position`设置MVP转换后的顶点数据

    vs中顶点数据经过MVP矩阵计算过后，会将顶点数据从三维空间转换至屏幕显示出来

    vs会被GPU调用执行，执行原理：vbo代表显存上的一个空间，与CPU不同，GPU的核心是非常多的（集成显卡一般有40几个核心，独显就好几百个），因此显卡的计算能力比CPU大很多，shader其实属于程序，需要再显卡中的核中运行的，当我们调用绘制指令绘制三角形的时候，三个顶点数据同时被GPU中的某三个核执行vertexShader, 只不过它们得到的数据不一样，是分别得到三个点。当执行完毕后，则执行fragmentShader阶段。所有shader在GPU上运行时并行的。

    fs中文件头最好加入兼容判断,保证该shader可以跨平台,示例：
    
    ``` c++
    #ifdef GL_ES
    precision mediump float; // 定义浮点数精度
    #endif
    ```
    
    fs中给`gl_FragColor`设置颜色或纹理贴图（可叠加），当过程到达这里的时候，opengl已经将三个点变成了面，因为这里要处理的是面这个问题，这个面的所有像素都会被fragment处理（同样并行），vertexShader处理的是三个顶点数据，fragment上是生成这三角形的所有像素（运算量巨大）

2. 编译shader

    流程：创建shader对象（指定种类），将源码设置至shader对象当中，让显卡对shader进行编译。（期间可监听编译状态并打印相关错误日志，方便调错）

3. 链接成程序

    流程：创建程序对象，将编译好的vs对象和fs对象绑定至程序上，链接程序，从程序上解绑两个shader对象。（期间可监听程序链接状态并打印相关错误日志，方便调错）

4. 获取程序中的变量（插槽）

    如何将文件中的顶点数据和MVP矩阵设置到shader当中（shader是运行在GPU上的）？
    
    因此需要获取shader中的变量（如attribute，uniform），并明确获取到的是什么东西。
    shader中有个插槽概念，插槽的排列是从0开始的，opengl会把变量与插槽对应上，例如：attribute会与对应该种变量的0号插槽，uniform就会对应uniform变量中的0~2号插槽，因此要设置这些变量即是告诉GPU哪个插槽放哪个数据。示例代码：
    
``` c++
// 声明插槽
GLint positionLocation, modelMatrixLocation;

positionLocation = glGetAttribLocation(program, "position"); // 参数为attribute的参数名
modelMatrixLocation = glGetUniformLocation(program, "ModelMatrix"); // 参数为uniform的参数名
```

5. 设置MVP矩阵，可以直接使用GLM中的mat4来创建MVP三个矩阵，投影矩阵需要初始化设置，另外两个矩阵直接是单位矩阵即可。

6. 绘制方法中：
    1. 使用创建好的shader程序，然后为程序设置uniform变量。如果有纹理设置，直接把要设置的纹理对象设置成为当前纹理(对于单个纹理来说)，把纹理对象与插槽对应起来即可。如果要设置多个纹理，由于shader中的属性都是与插槽相对应的，在纹理设置的时候它有额外的一组插槽叫纹理单元，而纹理对象则是图片中的像素，首先要先激活纹理单元（从0号位开始的）。示例代码：
    
    ``` c++
    // 声明一个容器存储多张纹理
    std::map<std::string, UniformTexture*> mUniformTextures;

    // 下面设置多个纹理对象
    int iIndex = 0;
    for (auto iter = mUniformTextures.begin(); iter != mUniformTextures.end(); iter++) {
        // 激活对应的纹理单元，另外：glActiveTexture(GL_TEXTURE0~N) = glActiveTexture(GL_TEXTURE0 + N),N为正整数
        glActiveTexture(GL_TEXTURE0 + iIndex);
        // 把纹理对象设置为当前的纹理对象，同时它也会指派给当前激活了的纹理单元
        glBindTexture(GL_TEXTURE_2D, iter->second->mTexture);
        // 设置好哪个插槽应该去第几个纹理单元中采集纹理，由于插槽是跟sampler2D关联上的，因此让sampler2D知道去哪里采集纹理
        glUniform1i(iter->second->mLocation, iIndex++);
    }
    ```

    2. 把vbo设置成绘制图形的数据集（shader在绘制时从vbo中取数据）, 分别启用attribute属性的插槽，并设置每个值的取值位置。

    3. 绘制图形。下面区分vbo和ebo不同的绘制方法示例：
    
    ``` c++

    // 三角形绘制

    // 把vbo设置成绘制图形的数据集,就是shader在绘制时从vbo中取数据
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // 绘制三角形，每3个点绘制成一个面，参数1：从第几个点开始；2是绘制多少个点
    glDrawArrays(GL_TRIANGLES, 0, 3); // 该方法在GPU端会遍历vbo中的数据，并且分别传入shader(shader已经分别拿到不同的点，它们的M,V,P矩阵是一模一样的，GPU中是有多个核心的，那么对于数据中的这3个点会同时地在某3个核中执行，)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 把ebo设置成绘制图形的数据集
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // 参数1是绘制三角形；2是取出ebo中的索引信息中的3个来绘制；3是数据类型; 4是数据起始位置
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 绘制执行完毕后，要把当前程序设置成0号程序
    glUseProgram(0);

    ```
## 面向对象封装
上面已经是使用shader进行GPU绘制的基本流程了，但是如果将上面的代码都统一写在全局初始化和绘制方法中，代码会非常臃肿。
下面是一些封装思路：

1. 将创建vbo或者ebo的过程封装成工具方法。注意：如果要创建ebo,首先要创建好vbo，再创建ebo来控制vbo中的点的绘制顺序。另外shader编译的流程同样可以封装。
``` c++
/**
 封装好创建vbo或者ebo的代码

 @param bufferType array_buffer/element_buffer
 @param size       buffer_size
 @param usage      static_draw/dynamic_draw
 @param data       data数据

 @return vbo对象或者ebo对象
 */
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data=nullptr);
```
2. VertexBuffer类（用于存储并设置顶点相关数据，存储vbo）

    1. 结构体：Vertex (包含position, color, texcoord, normal等)

    2. 顶点数据大对象（包含多个Vertex）

    3. vbo数据、设置顶点数据的、绑定vbo和解绑等方法

示例代码：

``` c++
struct Vertex{
    float position[4];
    float color[4];
    float texcoord[4];
    float normal[4];
};

class VertexBuffer{
public:
    Vertex *mVertexs; // 这里创建成指针对象是因为无法知道具体大小
    
    int mVertexCount; // 存储vb中有多少个点
    
    GLuint mVBO;
    
    /**
     设置vb大小

     @param vertexCount 点的数量
     */
    void setSize(int vertexCount);
    
    
    /**
     设置坐标

     @param index 第几个点
     @param x     x
     @param y     y
     @param z     z
     @param w     w
     */
    void setPosition(int index, float x, float y, float z, float w = 1.0f);
    
    /**
     设置颜色
     
     @param index 第几个点
     @param r     r
     @param g     g
     @param b     b
     @param a     a
     */
    void setColor(int index, float r, float g, float b, float a=1.0f);
    
    /**
     设置纹理坐标
     
     @param index 第几个点
     @param x     x
     @param y     y
     */
    void setTexcoord(int index, float x, float y);
    
    /**
     设置法线
     
     @param index 第几个点
     @param x     x
     @param y     y
     @param z     z
     */
    void setNormal(int index, float x, float y, float z);
    
    
    /**
     绑定当前vbo数据集
     */
    void bind();
    
    
    /**
     解绑当前vbo数据集
     */
    void unbind();
    
    
    /**
     获取某一个顶点的引用,用于外部修改

     @param index 索引

     @return 顶点数据
     */
    Vertex & get(GLuint index);
};

```

3. Shader类 （包括shader对象和程序的整个创建流程）

    1. 结构体1：UniformTexture 用于存储创建纹理对象时需要的插槽和纹理对象

    2. 结构体2：UniformVector4f 用于存储各种用于算法计算的vec4变量数据和对应插槽

    3. 程序对象、对应上面两个结构体类型的容器，属性插槽、MVP矩阵插槽，以及与这些对应的设置方法。
    
示例代码：

``` c++
struct UniformTexture {
    // 插槽
    GLint mLocation;
    // 纹理对象
    GLuint mTexture;
    
    UniformTexture(){
        mLocation = -1;
        mTexture = 0;
    }
};

// 添加对vector4的支持
struct UniformVector4f {
    // 插槽
    GLint mLocation;
    // uniform值
    float v[4];
    
    UniformVector4f(){
        mLocation = -1;
        // 初始化为0
        memset(v, 0, sizeof(float)*4);
    }
};

// 添加对立方体贴图的支持
struct UniformTextureCube {
    GLint mLocation;
    GLuint mTexture;
    
    UniformTextureCube() {
        mLocation = -1;
        mTexture = 0;
    }
};

class Shader{
public:
    
    // 声明程序
    GLint mProgram;
    
    // 至少支持一个纹理
//    UniformTexture mTexture;
    
    // 声明一个容器存储多张纹理
    std::map<std::string, UniformTexture*> mUniformTextures;
    
    std::map<std::string, UniformVector4f*> mUniformVec4s;
    
    std::map<std::string, UniformTextureCube*> mUniformTextureCubes;
    
    // 声明属性插槽
    GLint mPositionLocation, mColorLocation, mTexcoordLocation, mNormalLocation;
    
    // 声明矩阵插槽
    GLint mModelMatrixLocation, mViewMatrixLocation, mProjectionMatrixLocation;
    
    void init(const char*vs, const char*fs);
    
    // 绘制时需要的MVP矩阵
    void bind(float *M, float *V, float *P);
    
    
    /**
     设置图片纹理

     @param name      在shader中变量的名称
     @param imagePath 纹理图片地址
     */
    void setTexture(const char * name, const char * imagePath);
    
    
    /**
     设置程序纹理

     @param name    在shader中的变量名称
     @param texture 纹理对象
     */
    void setTexture(const char * name, GLuint texture);
    
    
    void setVec4(const char * name, float x, float y, float z, float w);
    
    
    /**
     设置立方体贴图纹理

     @param name    在shader中的变量名称
     @param texture 纹理对象

     @return oldTexture 旧纹理对象
     */
    GLuint setTextureCube(const char * name, GLuint texture);
    
};

```
    
4. Model类（统称，可根据类型划分多种类） （放置在场景中的所有模型，包括地面，天空盒，粒子等等）

    1. VertexBuffer对象、模型矩阵(M)、Shader对象、以及其他各个种类的模型所需要的特定属性
    2. 初始化方法、绘制方法、以及其他各个种类的模型所需要的特定方法

示例代码：

``` c++
class Model{
    
    VertexBuffer *mVertexBuffer;
    
    Shader *mShader;
    
public:
    
    // 模型缩放比例
    float scaleX, scaleY, scaleZ;
    
    // 物体当前世界坐标系
    glm::vec3 worldPos;
    
    // 物体当前视线方向
    glm::vec3 viewDirection;
    
    // 物体左右移动标识
    bool mMoveLeft, mMoveRight, mMoveForward, mMoveBack;
    
    // 是否开启键盘按键移动
    bool openKeyDownMove;
    
    // 将该矩阵放置于public中，因为draw接口无法满足很复杂的情况，直接由外部修改会更加方便
    glm::mat4 mModelMatrix;
    
    Model();
    
    void init(const char *modelPath);
    
    void draw(glm::mat4 &viewMatrix, glm::mat4 projectionMatrix, float x, float y, float z);
    
    void update(float frameTime);
    
    void setPosition(float x, float y, float z);
    
    void setScale(float x, float y, float z);
    
    void setRotate(float angle);
    
    void setAmbientMaterial(float r, float g, float b, float a);
    
    void setDiffuseMaterial(float r, float g, float b, float a);
    
    void setSpecularMaterial(float r, float g, float b, float a);
    
    void setTexture(const char *imagePath);
    
    void setTexture(const char *name, const char *imagePath);
    
    void movePosition(float deltaTime);
    
};
```

> 示例代码中有部分是与现在的FSViewer并不匹配（进行了新的方法处理），后续会在第二版进行升级，到时可以结合这里的文字并参阅实现源码进行分析。

## Shader中的光照示例：

1. 地面光照示例代码：

这里的地面光照示例已经大部分展示里顶点着色器和片元着色器在给环境添加各种光照时的基础应用情况，示例代码中已做注释和说明，具体参阅代码：

> vertexShader:

``` c++
attribute vec4 position;
attribute vec4 color;
attribute vec4 normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

varying vec4 V_Color;

varying vec3 V_Normal;
varying vec3 V_WorldPos;

// 由于地面的法线是垂直向上的，因此不需要使用IT_Matrix进行处理

void main(){
    V_Color = color;
    
    V_Normal = normal.xyz;
    
    V_WorldPos = (ModelMatrix * position).xyz;
    
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * position;
}
```

> fragmentShader: 

``` c++
#ifdef GL_ES

precision mediump float;

#endif

uniform vec4 U_LightPos; // 灯光位置
uniform vec4 U_LightAmbient; // 地面环境光分量
uniform vec4 U_LightDiffuse; // 地面漫反射分量
uniform vec4 U_AmbientMaterial; // 地面环境光反射系数
uniform vec4 U_DiffuseMaterial; // 地面漫反射系数

varying vec4 V_Color;

varying vec3 V_Normal;
varying vec3 V_WorldPos;

void main(){
    
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    
    // 设置点光源会使地面更加明显
    float distance = 0.0; //受光点与光源的距离
    float constantFactor = 1.0; // 常数衰减因子
    float linearFactor = 0.0; // 线性衰减因子
    float quadricFactor = 0.0; // 平方衰减因子
    
    vec3 L = U_LightPos.xyz - V_WorldPos; // 向量由地面指向光源位置
    distance = length(L); // 向量的模长
    
    // 根据点光源光照强度衰减公式
    float attenuation = 1.0 / constantFactor + (linearFactor * distance) + (quadricFactor * quadricFactor * distance);
    
    // 环境光颜色
    vec4 ambientColor = U_LightAmbient * U_AmbientMaterial;
    
    L = normalize(L);
    vec3 n = normalize(V_Normal.xyz);
    // 漫反射光照强度
    float diffuseIntensity = max(0.0, dot(L, n));
    // 漫反射颜色(初始亮度) * 衰减系数
    vec4 diffuseColor = U_LightDiffuse * U_DiffuseMaterial * diffuseIntensity * attenuation * 3.0;
    
    color = ambientColor + diffuseColor;
    
    // 光照颜色 * 顶点颜色
    gl_FragColor = color * V_Color;
    
}

```

2. 模型光照示例代码：

> vertexShader:

``` c++
attribute vec4 position;
attribute vec4 color;
attribute vec4 texcoord;
attribute vec4 normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// it_matrix, 对于无法确定物体的法向量时 ,如果没有用该矩阵处理过法线，物体的光效果有可能会出错，处理过后即100%没问题
uniform mat4 IT_ModelMatrix;

varying vec4 V_Color;

varying vec4 V_Normal; // 法线

varying vec4 V_WorldPos; // 表示当前点的世界坐标系上的位置

varying vec4 V_Texcoord;

void main(){
    V_Color = color;
    // 之所以在vs中用IT矩阵处理法向量因为在这里的处理的性能消耗较小，如果将此处理放在fs中则一般在法线贴图的时候才会用（因为涉及到大量的几何计算）
    V_Normal = IT_ModelMatrix * normal;
    
    // 世界坐标系上的点
    V_WorldPos = ModelMatrix * position;
    
    V_Texcoord = texcoord;
    
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * position;
}
```

> fragmentShader:

``` c++
#ifdef GL_ES

precision mediump float;

#endif

uniform sampler2D U_Texture;

uniform vec4 U_LightPos; // 光源位置（方向光）
uniform vec4 U_LightAmbient; // 环境光
uniform vec4 U_LightDiffuse; // 漫反射光分量

uniform vec4 U_LightSpecular; // 镜面反射光分量

uniform vec4 U_AmbientMaterial; // 模型表面材质对应环境光的反射系数
uniform vec4 U_DiffuseMaterial; // 材质对漫反射光的反射系数

uniform vec4 U_SpecularMaterial; // 材质的镜面反射系数
uniform vec4 U_CameraPos; // 摄像机的位置，镜面反射是根据视点来计算的，从不同角度看物体，物体的反光点是不一样的
uniform vec4 U_LightOpt; // 为了镜面反射光更加好看而使用的系数 / w分量作为控制是否纹理带高光效果

varying vec4 V_Color;

varying vec4 V_Normal;

varying vec4 V_WorldPos;

varying vec4 V_Texcoord;

// 给模型追加一个点光源
vec4 getPointLight() {
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
    
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0); // 定义一个纯黑色
    
    vec4 ambientColor = U_LightAmbient * U_AmbientMaterial;
    
    vec3 lightPos = U_LightPos.xyz; // ？
    
    vec3 L = lightPos; // 光源为方向光时，向量由物体指向光源位置
    L = normalize(L); // 单位化L
    
    vec3 n = normalize(V_Normal.xyz); // 单位化法线
    
    // 为了光线强度不要小于0，dot(L, n) 是 |L| * |n| * cos(L,n的夹角)，这就能定义光照强度(夹角越小，光照越强).原因是在方向光射至物体边缘的时候，L与n形成的夹角为90度，dot值为0，处于物体背对光源的位置的n与L的夹角大于90度，dot值为负数，因此使用max让其与0比较，因为物体背部的光照强度也为0.
    float diffuseIntensity = max(0.0, dot(L, n)); // 漫反射光强度
    
    // 设置镜面反射光为黑色
    vec4 specularColor = vec4(0.0, 0.0, 0.0, 0.0);
    // 只有当漫反射的光不为0的时候，物体才可能具备镜面反射光
    if(diffuseIntensity != 0.0){
        // 求出reflect反射光线向量
        vec3 reflectDir = normalize(reflect(-L, n));
        // 求出反射至照相机的向量
        vec3 viewDir = normalize(U_CameraPos.xyz - V_WorldPos.xyz);
        // 这里的光照强度取n次幂是为了使光照的强度呈非线性变化并由很强变很弱，从效果上来看就比较真实
        specularColor = U_LightSpecular * U_SpecularMaterial * pow(max(0.0, dot(viewDir, reflectDir)), U_LightOpt.x);
    }
    
    // 漫反射光照颜色为
    vec4 diffuseColor = U_LightDiffuse * U_DiffuseMaterial * diffuseIntensity;
    
    // 纹理和光照颜色混合有很多种，方式自选，下面示例两种：
    if(U_LightOpt.w == 1.0){
        color = ambientColor + diffuseColor * texture2D(U_Texture, V_Texcoord.xy) + specularColor;
    }else{
        color = ambientColor + diffuseColor + getPointLight();
        color = color * texture2D(U_Texture, V_Texcoord.xy);
    }
    
    // 物体表面颜色
//    color = ambientColor + diffuseColor + specularColor;
    
    gl_FragColor = color;
}

```

> 这里另外指出一个模拟镜面反射光的光照模型：blin光照，这种光照效率比上面的镜面反射要高很多，另外能处理当发光属性比较低时，视线方向与反射角度大于90度时光线反射成分被消除的问题，然而使光照效果更加真实并且在实际开发中通常采用这种光照代替镜面反射光照

> 这个光照模型的处理是不使用shader中reflect的内置方法，从reflect的原理上理解，对每一束光都求他的被反射过来的方向，这样的性能消耗非常大，因此我们可以采用视线方向与物体指向光源的方向相加，由向量相加的几何意义可以求得的模拟反射向量是比较接近reflect所求得的向量的，因此在顶点着色器中使用blin光照模型模拟镜面反射可以极大得降低性能的消耗

## 雾化

目前会制作的雾有两种，一种是`线性雾`，另外一种是`指数雾`

> 我们视线所看到的雾，本质是眼睛与物体间的光线受到大量粒子的扰动，一些原来直线传播无法进入眼睛的光线，被那些粒子反射而进入眼睛，这就是我们说看到成块状的微小粒子的的雾，视觉效果上是物体的颜色向雾的颜色偏移，粒子越大，偏移越大。

因此我们所得到的雾是有最近和最远的距离的，并且这个距离是以眼睛为标准的。

### 线性雾化
在线性雾化中，某一点的雾化程度取决于它与视点之间的距离，距离越远雾化程度越高。起点表示开始雾化，终点表示完全雾化，两点之间的某一点雾化程度与该视点的距离呈线性关系。某一点的雾化程度可以定义为`雾化因子`.

> `雾化因子 = (终点 - 当前点与视点的距离) / (终点 - 起点)， （起点 <= 当前点与视点的距离 <= 终点）`

如果雾化因子为1.0，该点完全没有被雾化，可以很清晰地看到此处的物体，如果是0.0，则该点完全被雾化，此处物体看不见。
由此可得fragmentShader中根据雾化因子计算片元的颜色则为：

> `片元颜色 = 物体表面颜色 x 雾化因子 + 雾的颜色 x (1 - 雾化因子)`

示例shader代码：
> vertexShader
``` c++
attribute vec4 a_Position;
attribute vec2 a_TexCoord;

uniform mat4 u_MVPMatrix;

uniform mat4 u_ModelMatrix;
uniform vec4 u_Eye;

varying float v_Dist;
varying vec2 v_TexCoord;

void main(){
	gl_Position = u_MVPMatrix * a_Position;
	v_TexCoord = a_TexCoord;
	// 计算顶点与视点之间的距离, 这样计算对性能消耗太大
	// v_Dist = distance(u_ModelMatrix * a_Position, u_Eye);
	v_Dist = gl_Position.w; // 这种性能虽好，但是会有一个不好的副作用，某一点的雾浓度可能因相机的朝向而改变，现实中不可能存在
}
```
> fragmentShader
``` c++
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_Sampler;
uniform vec3 u_FogColor; // 雾的颜色
uniform vec2 u_FogDist; // 雾的起点和终点

varying float v_Dist;
varying vec2 v_TexCoord;

void main(){
	// 计算雾化因子
	float fogFactor = clamp( (u_FogDist.y - v_Dist) / (u_FogDist.y - u_FogDist.x), 0.0, 1.0);
	// 纹理颜色
	vec4 texture = texture2D(u_Sampler, v_TexCoord);
	// 混合后的颜色, 这里mix公式: x * (1 - z) + y * z
	vec3 mixColor = mix(u_FogColor, vec3(texture.rgb), fogFactor);
	vec4 color = vec4(mixColor, 1.0);
	gl_FragColor = color;
}
```

### 指数雾

雾化的核心已经在线性雾中体现了，而指数雾实际上是表面雾本身是非线性变化的，而且是在不同方向上有可能也不一样，因此只要修改雾化因子公式即可。
这里增加一个缩放系数u_FogDist.z, 要想知道雾是如何变化的，自己绘制图像即可

` float fogFactor = clamp( exp(-(v_Dist * u_FogDist.z)), 0.0, 1.0); `

这里还有另外一种复杂度更高的雾，在u_FogDist.w = 1时，可等价于上面的雾

` float fogFactor = clamp( exp(-pow((v_Dist * u_FogDist.z),u_FogDist.w)), 0.0, 1.0); `













