//
//  skybox.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once
#include "vertexbuffer.h"
#include "shader.h"

class SkyBox{
    
    Shader *mShader;
    
    VertexBuffer *mVertexBuffer;
    
    glm::mat4 mModelMatrix;
    
public:
    
    bool isNight;
    
    void init(const char *imagePath, bool isNight = false);
    
    void initFront(const char *imagePath);
    
    void initBack(const char *imagePath);
    
    void initLeft(const char *imagePath);
    
    void initRight(const char *imagePath);
    
    void initTop(const char *imagePath);
    
    void initBottom(const char *imagePath);
    
    void draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, float x, float y, float z);
};
