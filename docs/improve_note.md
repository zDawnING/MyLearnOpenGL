# OpenGL进阶开发流程

在基础流程中，所有的绘图数据和编码都是在CPU上运行的，这种图形程序在较大的数据量中会产生很大的性能消耗，因此在进阶阶段要改变策略，将渲染代码上传至GPU上执行。
（后面看书后再重新更改）

> 环境：Mac OSX 系统, Xcode
> 使用类库：
>
>`libsoil`:OpenGL图像库,辅助处理各种格式的纹理贴图并生成纹理对象；
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
    3. varying 用于与fragmentShader共享数据，一般放置顶点颜色，纹理坐标等。
    
    `fragmentShader` 的基本属性类型：
    1. uniform: 一般放置纹理采样器（type为sampler2D）, 各种用于渲染的向量数据或光照参数等；
    2. varying: 同vertexShader

    vs中给`gl_Position`设置MVP转换后的顶点数据

    vs中顶点数据经过MVP矩阵计算过后，会将顶点数据从三维空间转换至屏幕显示出来

    vs会被GPU调用执行，执行原理：vbo代表显存上的一个空间，与CPU不同，GPU的核心是非常多的（集成显卡一般有40几个核心，独显就好几百个），因此显卡的计算能力比CPU大很多，shader其实属于程序，需要再显卡中的核中运行的，当我们调用绘制指令绘制三角形的时候，三个顶点数据同时被GPU中的某三个核执行vertexShader, 只不过它们得到的数据不一样，是分别得到三个点。当执行完毕后，则执行fragmentShader阶段。所有shader在GPU上运行时并行的。

    fs中文件头最好加入兼容判断,保证该shader可以跨平台,示例：
    ```
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
    1. 使用创建好的shader程序，然后为程序设置uniform变量。如果有纹理设置，直接把要设置的纹理对象设置成为当前纹理，把纹理对象与插槽对应起来即可。

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

1. 将创建vbo或者ebo的过程封装成工具方法。注意：如果要创建ebo,首先要创建好vbo，再创建ebo来控制vbo中的点的绘制顺序。
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
    

3. Shader类 （包括shader对象和程序的整个创建流程）

    1. 结构体1：UniformTexture 用于存储创建纹理对象时需要的插槽和纹理对象

    2. 结构体2：UniformVector4f 用于存储各种用于算法计算的vec4变量数据和对应插槽

    3. 程序对象、对应上面两个结构体类型的容器，属性插槽、MVP矩阵插槽，以及与这些对应的设置方法。
    
    
4. Model类（统称，可根据类型划分多种类） （放置在场景中的所有模型，包括地面，天空盒，粒子等等）
 
    1. VertexBuffer对象、模型矩阵(M)、Shader对象、以及其他各个种类的模型所需要的特定属性

    2. 初始化方法、绘制方法、以及其他各个种类的模型所需要的特定方法
    


