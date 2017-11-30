//
//  particlesystem.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/29.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "particlesystem.h"
#include "utils.h"

ParticleSystem::ParticleSystem(){
    mLifeTime = -1.0f; // 这里让每新建出来的particle对象都有一个生命标识
}

void ParticleSystem::init(float x, float y, float z){
    // 设置粒子位置
    mModelMatrix = glm::translate(x, y, z);
    
    
    mVertexBuffer = new VertexBuffer;
//    mVertexBuffer->setSize(1);
//    mVertexBuffer->setColor(0, 0.1f, 0.4f, 0.6f);
    
    
    
    int particleCount = 16;
    int groudIndex = 1; // 按4颗分一组
    mVertexBuffer->setSize(particleCount);
    for (int i=0; i<particleCount; i++) {
        if(i % 4 == 0 && groudIndex > 1){
            mVertexBuffer->setPosition(i, x + 1.5f * cosf((float)i * 60.0 * (M_PI / 180.0f)), y - 1.5, z + 1.5f * sinf(i * 60.0 * (M_PI / 180.0f)));
            switch (groudIndex) {
                case 2:
                    mVertexBuffer->setColor(i, 0.8f, 0.0f, 0.0f);
                    break;
                case 3:
                    mVertexBuffer->setColor(i, 0.0f, 0.73f, 0.83f);
                    break;
                case 4:
                    mVertexBuffer->setColor(i, 1.0f, 0.75f, 0.02f);
                    break;
            }
            groudIndex++;
        }
        if(i % 4 == 0 && groudIndex == 1){
            mVertexBuffer->setPosition(i, x, y, z);
            mVertexBuffer->setColor(i, 1.0f, 1.0f, 1.0f);
            groudIndex++;
        }
    }
    
    mShader = new Shader;
    mShader->init("Res/shader/particle.vs", "Res/shader/particle.fs");
    mShader->setTexture("U_Texture", CreateProcedureTexture(128));
}

void ParticleSystem::draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix){
    // 启用程序控制点精灵的大小
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_DEPTH_TEST);
    // 粒子带alpha通道，这里启用alpha混合，解决有锯齿状不平滑的边界
    glEnable(GL_BLEND);
    // GL_ONE会让某一个像素点的颜色越来越亮，叠加越多越亮
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    mVertexBuffer->bind();
    mShader->bind(glm::value_ptr(mModelMatrix), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix));
    glDrawArrays(GL_POINTS, 0, mVertexBuffer->mVertexCount);
    mVertexBuffer->unbind();
    
    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
}



void ParticleSystem::update(float deltaTime){
    // 声明记录粒子系统的旋转角度（是整个粒子系统在旋转）
    static float angle = 0.0;
    // 每一帧旋转一定的角度
    angle += deltaTime * 80.0f;
    mModelMatrix = glm::rotate(angle, 0.0f, 1.0f, 0.0f);
}

void ParticleSystem::setPosition(float x, float y, float z){
    worldPos = glm::vec3(x, y, z);
    mModelMatrix *= glm::translate(x, y, z);
    
}

void ParticleSystem::updateLaunch(int index, float deltaTime){
    if(mLivingTime > mLifeTime){ // 凋亡的不更新
        return;
    }
    mLivingTime += deltaTime; // 时间流逝，寿命增加
    float maxRadius = 5.0f; // 最大可旋转至的半径
    float rotateSpeed = 36.0f; // 设置旋转速度
    float currentAngle = rotateSpeed * mLivingTime; // 当前的旋转弧度 = 旋转速度 * 已生存的时间
    float x = maxRadius * cosf(currentAngle*M_PI / 180.0f);
    float z = maxRadius * sinf(currentAngle*M_PI / 180.0f);
    mVertexBuffer->setPosition(index, x, 3.0f, z);
}





