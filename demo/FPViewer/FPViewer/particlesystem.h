//
//  particlesystem.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/29.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once

#include "vertexbuffer.h"
#include "shader.h"


class ParticleSystem{
    
    VertexBuffer *mVertexBuffer;
    
    glm::mat4 mModelMatrix;
    
    Shader *mShader;
    
public:
    
    glm::vec3 worldPos;
    
    ParticleSystem();
    
    float mLifeTime; // 生命值（该粒子能存在多久）
    
    float mLivingTime; // 活了多久（该粒子已经存在多长时间）
    
    void init(float x, float y, float z);
    
    void draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix);
    
    void update(float deltaTime);
    
    void setPosition(float x, float y, float z);
    
    void updateLaunch(int index, float deltaTime);
};
