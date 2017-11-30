//
//  ground.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once

#include "utils.h"
#include "vertexbuffer.h"
#include "shader.h"

class Ground{
    
    VertexBuffer *mVertexBuffer;
    
    // 添加shader
    Shader *mShader;
    
    // 添加该物体对象的modelMatrix (物体自身的)
    glm::mat4 mModelMatrix;
    
public:
    
    bool isNight;
    
    void init(bool isNight = false);
    
    void draw(glm::mat4 & viewMatrix, glm::mat4 & projectionMatrix);
    
    void setTexture(const char *imagePath);
};
