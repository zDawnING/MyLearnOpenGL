//
//  ground.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "ground.h"

void Ground::init(bool isNight){
    mVertexBuffer = new VertexBuffer;
    // 创建400个砖块
    mVertexBuffer->setSize(1600);
    
    float yCoord = - 1.0;
    
    for (int z=0; z<20; z++) {
        float zStart = 100.0f - z*10.0f;
        for (int x=0; x<20; x++) {
            float xStart = x*10.0f - 100.0f;
            int offset = (x + z * 20) * 4;
            // 设置顶点位置
            mVertexBuffer->setPosition(offset, xStart, yCoord, zStart);
            mVertexBuffer->setPosition(offset+1, xStart+10.0f, yCoord, zStart);
            mVertexBuffer->setPosition(offset+2, xStart, yCoord, zStart-10.0f);
            mVertexBuffer->setPosition(offset+3, xStart+10.0f, yCoord, zStart-10.0f);
            // 设置法线
            mVertexBuffer->setNormal(offset, 0.0f, 1.0f, 0.0f);
            mVertexBuffer->setNormal(offset+1, 0.0f, 1.0f, 0.0f);
            mVertexBuffer->setNormal(offset+2, 0.0f, 1.0f, 0.0f);
            mVertexBuffer->setNormal(offset+3, 0.0f, 1.0f, 0.0f);
            // 设置纹理坐标
            mVertexBuffer->setTexcoord(offset, 0.0f, 0.0f);
            mVertexBuffer->setTexcoord(offset+1, 1.0f, 0.0f);
            mVertexBuffer->setTexcoord(offset+2, 0.0f, 1.0f);
            mVertexBuffer->setTexcoord(offset+3, 1.0f, 1.0f);
        }
    }
    
    
    mShader = new Shader;
    mShader->init("Res/shader/ground.vs", "Res/shader/ground.fs");
    
    if(isNight){
        // 设置夜间灯光模式
        
        mShader->setVec4f("U_LightPos", 0.0f, 1.0f, 1.0f, 0.0f);
        mShader->setVec4f("U_LightAmbient", 2.0f, 2.0f, 2.0f, 2.0f);
        mShader->setVec4f("U_LightDiffuse", 0.1f, 0.4f, 0.6f, 1.0f);
        mShader->setVec4f("U_AmbientMaterial", 0.1f, 0.1f, 0.1f, 1.0f);
        mShader->setVec4f("U_DiffuseMaterial", 0.6f, 0.6f, 0.6f, 1.0f);
    }else{
        mShader->setVec4f("U_LightPos", 0.0f, 3.0f, 1.0f, 0.0f);
        mShader->setVec4f("U_LightAmbient", 1.0f, 1.0f, 1.0f, 1.0f);
        mShader->setVec4f("U_LightDiffuse", 0.3f, 0.3f, 0.3f, 1.0f);
        mShader->setVec4f("U_AmbientMaterial", 0.8f, 0.8f, 0.8f, 1.0f);
        mShader->setVec4f("U_DiffuseMaterial", 0.8f, 0.8f, 0.8f, 1.0f);
    }
    
}

void Ground::draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix){
    glEnable(GL_DEPTH_TEST);
    mVertexBuffer->bind();
    mShader->bind(glm::value_ptr(mModelMatrix), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix));
    // 绘制400个方块
    for (int i=0; i<400; i++) {
        glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
    }
    mVertexBuffer->unbind();
}

void Ground::setTexture(const char *imagePath){
    mShader->setTexture("U_Texture", imagePath);
}

