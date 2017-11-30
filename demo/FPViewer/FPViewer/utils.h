//
//  utils.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/22.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once
#include "ggl.h"

/**
 使用NSData来加载文件
 
 @param path     文件相对地址
 @param fileSize 文件大小
 
 @return 文件内容指针
 */
unsigned char * LoadFileContent(const char *path, int &fileSize);


/**
 从图片中创建2D纹理，利用soil库进行解析图片文件

 @param imagePath 图片地址
 @param inverY    图片是否翻转（默认将图片翻转）

 @return 纹理对象
 */
GLuint CreateTextureFromImg(const char *imagePath, bool inverY=true);

/**
 解码bmp图片
 
 @param bmpFileData 文件内容的内存块
 @param width   图片的宽
 @param height  图片的高
 
 @return 返回新的指针
 */
unsigned char* DecodeBMP(unsigned char*bmpFileData, int& width, int& height);

/**
 将像素数据解析并生成纹理
 
 @param pixelData 像素数据
 @param width     width
 @param height    height
 @param type      纹理在显卡上的数据类型
 
 @return 纹理对象
 */
GLuint CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type);

/**
 封装完善创建纹理
 
 @param bmpPath bmp文件路径
 
 @return 返回纹理对象标识
 */
GLuint CreateTexture2DFromBMP(const char* bmpPath);


/**
 创建程序纹理，这里仅创建一个正方形纹理

 @param size 高宽

 @return 纹理对象
 */
GLuint CreateProcedureTexture(int size);

/**
 获取每一帧花费的时间，就是两帧直接的间隔
 
 @return 间隔时间
 */
float GetFrameTime();

/**
 封装好创建vbo或者ebo的代码
 
 @param bufferType array_buffer/element_buffer
 @param size       buffer_size
 @param usage      static_draw/dynamic_draw
 @param data       data数据
 
 @return gluint
 */
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data=nullptr);

/**
 编译shader
 
 @param shaderType shader类型（vertex/fragment）
 @param shaderCode shader代码（shader资源中的代码）
 
 @return gluint
 */
GLuint CompileShader(GLenum shaderType, const char *shaderCode);

/**
 把shader链接成程序，用于生成绘制图形的程序
 
 @param vs 编译成功的vertexShader
 @param fs 编译成功的fragmentShader
 
 @return gluint
 */
GLuint CreateProgram(GLuint vs, GLuint fs);



