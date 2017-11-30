//
//  model.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once

#pragma once

#include "ggl.h"
#include "vertexbuffer.h"
#include "shader.h"

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
