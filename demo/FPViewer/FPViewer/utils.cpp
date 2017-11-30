//
//  utils.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/22.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "utils.h"
#include "soil.h"

/**
 从图片中创建2D纹理，利用soil库进行解析图片文件
 
 @param imagePath 图片地址
 @param inverY    图片是否翻转（默认将图片翻转）
 
 @return 纹理对象
 */
GLuint CreateTextureFromImg(const char *imagePath, bool inverY){
    int nFileSize = 0;
    unsigned char *fileContent = LoadFileContent(imagePath, nFileSize);
    if(fileContent == nullptr){
        return 0;
    }
    
    unsigned int flags = SOIL_FLAG_TEXTURE_REPEATS;
    
    if(inverY){ // 如果需要翻转y则添加标记
        flags = SOIL_FLAG_INVERT_Y;
    }
    // 从内存中加载一个纹理对象出来, 参数是：内存块，块大小，图片有多少个颜色通道（如rgb或者rgba,0则表示根据图片自身来定），是否重用某一个纹理对象，0为新增一个（如果要重用对象，会写入图片的内容覆盖至纹理对象中）
    GLuint texture = SOIL_load_OGL_texture_from_memory(fileContent, nFileSize, 0, 0, flags);
    delete fileContent;
    return texture;
};


/**
 解码bmp图片
 
 @param bmpFileData 文件内容的内存块
 @param width   图片的宽
 @param height  图片的高
 
 @return 返回新的指针
 */
unsigned char* DecodeBMP(unsigned char*bmpFileData, int& width, int& height) {
    // 判断是否是一个位图
    if( 0x4D42 == *(unsigned short*)bmpFileData ){
        int pixelDataOffset = *( (int*)(bmpFileData + 10) ); // 10字节处取整形是位图的像素数据在整个位图内存块当中的偏移地址
        width = *( (int*)(bmpFileData + 18) ); // 位图的宽
        height = *( (int*)(bmpFileData + 22) ); // 位图的高
        unsigned char*pixelData = bmpFileData + pixelDataOffset; // 新建一个临时指针指向位图的像素内存块的起始位置
        // 对像素块进行转码, 位图的像素存储格式为（b,g,r）的形式，opengl中仅识别（r,g,b）格式的像素数据，因此要做数据转换，另外这里解码的问题无法支持alpha通道，该方法是为了单独解决天空盒缝隙问题而单独读取图片像素做处理
        for (int i=0; i<width * height * 3; i+= 3) {
            unsigned char temp = pixelData[i];
            pixelData[i] = pixelData[i + 2];
            pixelData[i + 2] = temp;
        }
        return pixelData;
    }
    return nullptr;
}


/**
 将像素数据解析并生成纹理

 @param pixelData 像素数据
 @param width     width
 @param height    height
 @param type      纹理在显卡上的数据类型

 @return 纹理对象
 */
GLuint CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type) {
    GLuint texture;
    glGenTextures(1, &texture);
    // 设置为当前的纹理对象
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // filter则是当纹理被放大时怎样去采集像素（小纹理贴在大物体上的着色问题），linear使用线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 这里与上面相反，大纹理贴在小物体上的着色处理
    
    // 接下来设置纹理坐标使用的参数, 这里使用的是当设置的几何形体的ua坐标超过1.0的时候应该如何采集纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // clamp表示超过1.0后，取纹理边界像素
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE 可以解决天空盒的缝隙问题
    
    // 将像素数据上传至显卡GPU中，将内存中的数据拷贝至显存（该方法消耗内存较大，后面研究改进新的处理形式，或者变成使用库通用生成纹理）
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixelData);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

/**
 封装完善创建纹理
 
 @param bmpPath bmp文件路径
 
 @return 返回纹理对象标识
 */
GLuint CreateTexture2DFromBMP(const char* bmpPath) {
    int nFileSize = 0;
    unsigned char*bmpFileContent = LoadFileContent(bmpPath, nFileSize);
    if (bmpFileContent==nullptr){
        return 0; // 0号纹理单元是默认黑色的（opengl提供）
    }
    int bmpWidth = 0, bmpHeight = 0;
    unsigned char*pixelData = DecodeBMP(bmpFileContent, bmpWidth, bmpHeight);
    if (bmpWidth==0){
        delete bmpFileContent;
        return 0;
    }
    GLuint texture= CreateTexture2D(pixelData, bmpWidth, bmpHeight, GL_RGB);
    delete bmpFileContent;
    return texture;
}

/**
 封装好创建vbo或者ebo的代码
 
 @param bufferType array_buffer/element_buffer
 @param size       buffer_size
 @param usage      static_draw/dynamic_draw
 @param data       data数据
 
 @return gluint
 */
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data /*=nullptr*/){
    GLuint object;
    // 生成一个buffer_obj
    glGenBuffers(1, &object);
    glBindBuffer(bufferType, object);
    glBufferData(bufferType, size, data, usage);
    glBindBuffer(bufferType, 0);
    return object;
}

/**
 编译shader
 
 @param shaderType shader类型（vertex/fragment）
 @param shaderCode shader代码（shader资源中的代码）
 
 @return shader对象
 */
GLuint CompileShader(GLenum shaderType, const char *shaderCode){
    // 创建shader
    GLuint shader = glCreateShader(shaderType);
    // 设置源码
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);
    // 监听编译状态
    GLint compileResult = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
    if(compileResult == GL_FALSE){ // 编译失败
        char log[1024] = {0};
        GLsizei logLen = 0;
        glGetShaderInfoLog(shader, 1024, &logLen, log);
        printf("Compile Shader fail error log: %s \n, shader code \n%s\n", log, shaderCode );
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}

/**
 把shader链接成程序，用于生成绘制图形的程序
 
 @param vs 编译成功的vertexShader
 @param fs 编译成功的fragmentShader
 
 @return gluint
 */
GLuint CreateProgram(GLuint vs, GLuint fs){
    // 创建程序
    GLuint program = glCreateProgram();
    // 绑定shader至程序
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    // 链接程序
    glLinkProgram(program);
    // 解绑shader
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    // 监听创建程序状态
    GLint nResult;
    glGetProgramiv(program, GL_LINK_STATUS, &nResult);
    if(nResult == GL_FALSE){
        char log[1024] = {0};
        GLsizei logLen = 0;
        glGetProgramInfoLog(program, 1024, &logLen, log);
        printf("Create GPU program fail, link error: %s\n", log);
        glDeleteProgram(program);
        program = 0;
    }
    return program;
}

/**
 创建程序纹理，这里仅创建一个正方形纹理
 
 @param size 高宽
 
 @return 纹理对象
 */
GLuint CreateProcedureTexture(int size){
    // 像素数据集
    unsigned char *imageData = new unsigned char[size * size * 4];
    float halfSize = (float)size / 2.0f;
    // 离纹理中心的最大距离
    float maxDistance = sqrtf(halfSize*halfSize + halfSize*halfSize);
    float centerX = halfSize;
    float centerY = halfSize;
    for (int y=0; y<size; y++) {
        for (int x=0; x<size; x++) {
            int currPixelOffset = (x + y*size) * 4;
            imageData[currPixelOffset] = 255;
            imageData[currPixelOffset+1] = 255;
            imageData[currPixelOffset+2] = 255;
            // 求出当前像素离中心轴的距离
            float deltaX = (float)x - centerX;
            float deltaY = (float)y - centerY;
            // 利用勾股定理
            float distance = sqrtf(deltaX*deltaX + deltaY*deltaY);
            // 先将alph值呈线性变化（根据式子观察函数图像），然后求其幂函数叠加非线性变化
            float alpha = powf(1.0 - (distance / maxDistance), 8.0f);
            // 容错处理
            alpha = alpha > 1.0 ? 1.0 : alpha;
            imageData[currPixelOffset+3] = (unsigned char)(alpha * 255.0f);
        }
    }
    GLuint texture = CreateTexture2D(imageData, size, size, GL_RGBA);
    delete imageData;
    return texture;
}









