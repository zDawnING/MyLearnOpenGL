//
//  shader.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once
#include "ggl.h"

using namespace std;

struct UniformTexture{
    GLint mLocation;
    GLuint mTexture;
    
    UniformTexture(){
        mLocation = -1;
        mTexture = 0;
    }
};

struct UniformVector4f{
    GLint mLocation;
    float vec4f[4];
    
    UniformVector4f(){
        mLocation = -1;
        memset(vec4f, 0, sizeof(float) * 4);
    }
};

class Shader{
    
public:
    
    GLint mProgram;
    
    map<string, UniformTexture*> mUniformTextures;
    
    map<string, UniformVector4f*> mUniformVec4fs;
    
    // 属性插槽
    GLint mPositionLocation, mColorLocation, mTexcoordLocation, mNormalLocation;
    
    // 矩阵插槽
    GLint mModelMatrixLocation, mViewMatrixLocation, mProjectionMatrixLocation;
    
    void init(const char *vs, const char *fs);
    
    void bind(float *M, float *V, float *P);
    
    /**
     设置图片纹理
     
     @param name      在shader中变量的名称
     @param imagePath 纹理图片地址
     */
    void setTexture(const char *name, const char * imagePath);
    
    /**
     为天空盒设置纹理，为了解决天空盒缝隙问题
     
     @param name      在shader中变量的名称
     @param imagePath 纹理图片地址
     */
    void setTextureToSkyBox(const char *name, const char * imagePath);
    
    /**
     设置程序纹理
     
     @param name    在shader中的变量名称
     @param texture 纹理对象
     */
    void setTexture(const char *name, GLuint texture);
    
    
    void setVec4f(const char *name, float x, float y, float z, float w);
    
};
