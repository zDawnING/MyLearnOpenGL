//
//  shader.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "shader.h"
#include "utils.h"
#include "vertexbuffer.h"

void Shader::init(const char *vs, const char *fs){
    // 读取shader文件代码
    int fileSize = 0;
    const char *vsCode = (char*)LoadFileContent(vs, fileSize);
    const char *fsCode = (char*)LoadFileContent(fs, fileSize);
    // 编译shader
    GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsCode);
    if(vsShader == 0){
        return;
    }
    GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsCode);
    if(fsShader == 0){
        return;
    }
    
    mProgram = CreateProgram(vsShader, fsShader);
    glDeleteShader(vsShader);
    glDeleteShader(fsShader);
    
    if(mProgram != 0){
        mPositionLocation = glGetAttribLocation(mProgram, "position");
        mColorLocation = glGetAttribLocation(mProgram, "color");
        mTexcoordLocation = glGetAttribLocation(mProgram, "texcoord");
        mNormalLocation = glGetAttribLocation(mProgram, "normal");
        mModelMatrixLocation = glGetUniformLocation(mProgram, "ModelMatrix");
        mViewMatrixLocation = glGetUniformLocation(mProgram, "ViewMatrix");
        mProjectionMatrixLocation = glGetUniformLocation(mProgram, "ProjectionMatrix");
    }
}

void Shader::bind(float *M, float *V, float *P){
    glUseProgram(mProgram);
    // 设置MVP矩阵值至对应插槽
    glUniformMatrix4fv(mModelMatrixLocation, 1, GL_FALSE, M);
    glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, V);
    glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, P);
    
    // 设置容器中的纹理对象
    int iIndex = 0;
    for (auto iter = mUniformTextures.begin() ; iter != mUniformTextures.end(); iter++) {
        // 激活纹理单元
        glActiveTexture(GL_TEXTURE0 + iIndex);
        glBindTexture(GL_TEXTURE_2D, iter->second->mTexture);
        glUniform1i(iter->second->mLocation, iIndex++);
    }
    
    // 设置vec4
    for (auto iter = mUniformVec4fs.begin(); iter != mUniformVec4fs.end(); iter++) {
        glUniform4fv(iter->second->mLocation, 1, iter->second->vec4f);
    }
    
    // 这里设置的GPU读取vbo内存块的方式欠妥，需要后面找到更合适的方面，另外需要了解vbo中内存块的排布原理，后面再加以改进，
    // 这里目前要保证Vertex类中的属性数组排布方式与值正确对应，否则绘制就会发生各种奇葩效果
    glEnableVertexAttribArray(mPositionLocation);
    glVertexAttribPointer(mPositionLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(mColorLocation);
    glVertexAttribPointer(mColorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*4));
    glEnableVertexAttribArray(mTexcoordLocation);
    glVertexAttribPointer(mTexcoordLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*8));
    glEnableVertexAttribArray(mNormalLocation);
    glVertexAttribPointer(mNormalLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float)*12));
    
}

/**
 设置图片纹理
 
 @param name      在shader中变量的名称
 @param imagePath 纹理图片地址
 */
void Shader::setTexture(const char *name, const char * imagePath){
    // 从容器中查找是否有对应名字的纹理单元
    auto iter = mUniformTextures.find(name);
    if(iter == mUniformTextures.end()){ // 没有找到
        // 找到插槽
        GLint location = glGetUniformLocation(mProgram, name);
        if(location != -1){
            // 创建纹理对象
            UniformTexture *t = new UniformTexture;
            t->mLocation = location;
            t->mTexture = CreateTextureFromImg(imagePath);
            // 插入至容器当中，这里使用了对组(pair)组合成一个map值，故访问时需要指定first/second
            mUniformTextures.insert(pair<string, UniformTexture*>(name, t));
        }
    }else{ // 找到则重新设置纹理像素即可
        // 删除原有
        glDeleteTextures(1, &iter->second->mTexture);
        iter->second->mTexture = CreateTextureFromImg(imagePath);
    }
}

/**
 为天空盒设置纹理，为了解决天空盒缝隙问题
 
 @param name      在shader中变量的名称
 @param imagePath 纹理图片地址
 */
void Shader::setTextureToSkyBox(const char *name, const char * imagePath){
    // 从容器中查找是否有对应名字的纹理单元
    auto iter = mUniformTextures.find(name);
    if(iter == mUniformTextures.end()){ // 没有找到
        // 找到插槽
        GLint location = glGetUniformLocation(mProgram, name);
        if(location != -1){
            // 创建纹理对象
            UniformTexture *t = new UniformTexture;
            t->mLocation = location;
            t->mTexture = CreateTexture2DFromBMP(imagePath);
            // 插入至容器当中，这里使用了对组(pair)组合成一个map值，故访问时需要指定first/second
            mUniformTextures.insert(pair<string, UniformTexture*>(name, t));
        }
    }else{ // 找到则重新设置纹理像素即可
        // 删除原有
        glDeleteTextures(1, &iter->second->mTexture);
        iter->second->mTexture = CreateTexture2DFromBMP(imagePath);
    }
}

/**
 设置程序纹理
 
 @param name    在shader中的变量名称
 @param texture 纹理对象
 */
void Shader::setTexture(const char *name, GLuint texture){
    auto iter = mUniformTextures.find(name);
    if(iter == mUniformTextures.end()){
        GLint location = glGetUniformLocation(mProgram, name);
        if(location != -1){
            UniformTexture *t = new UniformTexture;
            t->mLocation = location;
            t->mTexture = texture;
            mUniformTextures.insert(pair<string, UniformTexture*>(name, t));
        }
    }else{
        glDeleteTextures(1, &iter->second->mTexture);
        iter->second->mTexture = texture;
    }
}


void Shader::setVec4f(const char *name, float x, float y, float z, float w){
    // 查找是否设置过该名称的vec4
    auto iter = mUniformVec4fs.find(name);
    if(iter == mUniformVec4fs.end()){
        GLint location = glGetUniformLocation(mProgram, name);
        if(location != -1){
            UniformVector4f *t = new UniformVector4f;
            t->mLocation = location;
            t->vec4f[0] = x;
            t->vec4f[1] = y;
            t->vec4f[2] = z;
            t->vec4f[3] = w;
            mUniformVec4fs.insert(pair<string, UniformVector4f*>(name, t));
        }
    }else{ // 已设置过vec4,即更新值
        iter->second->vec4f[0] = x;
        iter->second->vec4f[1] = y;
        iter->second->vec4f[2] = z;
        iter->second->vec4f[3] = w;
    }
}





