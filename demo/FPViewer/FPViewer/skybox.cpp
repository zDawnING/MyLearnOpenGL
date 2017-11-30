//
//  skybox.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "skybox.h"

void SkyBox::init(const char *imagePath, bool isNight){
    mVertexBuffer = new VertexBuffer[6];
    mShader = new Shader[6];
    
    this->isNight = isNight;
   
    initFront(imagePath);
    initBack(imagePath);
    initLeft(imagePath);
    initRight(imagePath);
    initTop(imagePath);
    initBottom(imagePath);
}

void SkyBox::initFront(const char *imagePath){
    mShader[0].init("Res/shader/skybox.vs", "Res/shader/skybox.fs");
    char temp[256];
    memset(temp, 0, 256);
    strcpy(temp, imagePath);
    strcat(temp, "front.bmp");
    
    mShader[0].setTextureToSkyBox("U_Texture", temp);
    
    mVertexBuffer[0].setSize(4);
    // 绘制方式是gl_triangle_strip，注意点的绘制方式
    mVertexBuffer[0].setPosition(0, -0.5f, -0.5f, -0.5f);
    mVertexBuffer[0].setTexcoord(0, 0.0f, 0.0f);
    mVertexBuffer[0].setPosition(1, 0.5f, -0.5f, -0.5f);
    mVertexBuffer[0].setTexcoord(1, 1.0f, 0.0f);
    mVertexBuffer[0].setPosition(2, -0.5f, 0.5f, -0.5f);
    mVertexBuffer[0].setTexcoord(2, 0.0f, 1.0f);
    mVertexBuffer[0].setPosition(3, 0.5f, 0.5f, -0.5f);
    mVertexBuffer[0].setTexcoord(3, 1.0f, 1.0f);
}

void SkyBox::initBack(const char *imagePath){
    mShader[1].init("Res/shader/skybox.vs", "Res/shader/skybox.fs");
    char temp[256];
    memset(temp, 0, 256);
    strcpy(temp, imagePath);
    strcat(temp, "back.bmp");
    
    mShader[1].setTextureToSkyBox("U_Texture", temp);
    
    mVertexBuffer[1].setSize(4);
    mVertexBuffer[1].setPosition(0, 0.5f, -0.5f, 0.5f);
    mVertexBuffer[1].setTexcoord(0, 0.0f, 0.0f);
    mVertexBuffer[1].setPosition(1, -0.5f, -0.5f, 0.5f);
    mVertexBuffer[1].setTexcoord(1, 1.0f, 0.0f);
    mVertexBuffer[1].setPosition(2, 0.5f, 0.5f, 0.5f);
    mVertexBuffer[1].setTexcoord(2, 0.0f, 1.0f);
    mVertexBuffer[1].setPosition(3, -0.5f, 0.5f, 0.5f);
    mVertexBuffer[1].setTexcoord(3, 1.0f, 1.0f);
}

void SkyBox::initLeft(const char *imagePath){
    mShader[2].init("Res/shader/skybox.vs", "Res/shader/skybox.fs");
    char temp[256];
    memset(temp, 0, 256);
    strcpy(temp, imagePath);
    strcat(temp, "left.bmp");
    
    mShader[2].setTextureToSkyBox("U_Texture", temp);
    
    mVertexBuffer[2].setSize(4);
    mVertexBuffer[2].setPosition(0, -0.5f, -0.5f, 0.5f);
    mVertexBuffer[2].setTexcoord(0, 0.0f, 0.0f);
    mVertexBuffer[2].setPosition(1, -0.5f, -0.5f, -0.5f);
    mVertexBuffer[2].setTexcoord(1, 1.0f, 0.0f);
    mVertexBuffer[2].setPosition(2, -0.5f, 0.5f, 0.5f);
    mVertexBuffer[2].setTexcoord(2, 0.0f, 1.0f);
    mVertexBuffer[2].setPosition(3, -0.5f, 0.5f, -0.5f);
    mVertexBuffer[2].setTexcoord(3, 1.0f, 1.0f);
}

void SkyBox::initRight(const char *imagePath){
    mShader[3].init("Res/shader/skybox.vs", "Res/shader/skybox.fs");
    char temp[256];
    memset(temp, 0, 256);
    strcpy(temp, imagePath);
    strcat(temp, "right.bmp");
    
    mShader[3].setTextureToSkyBox("U_Texture", temp);
    
    mVertexBuffer[3].setSize(4);
    mVertexBuffer[3].setPosition(0, 0.5f, -0.5f, -0.5f);
    mVertexBuffer[3].setTexcoord(0, 0.0f, 0.0f);
    mVertexBuffer[3].setPosition(1, 0.5f, -0.5f, 0.5f);
    mVertexBuffer[3].setTexcoord(1, 1.0f, 0.0f);
    mVertexBuffer[3].setPosition(2, 0.5f, 0.5f, -0.5f);
    mVertexBuffer[3].setTexcoord(2, 0.0f, 1.0f);
    mVertexBuffer[3].setPosition(3, 0.5f, 0.5f, 0.5f);
    mVertexBuffer[3].setTexcoord(3, 1.0f, 1.0f);

}

void SkyBox::initTop(const char *imagePath){
    mShader[4].init("Res/shader/skybox.vs", "Res/shader/skybox.fs");
    char temp[256];
    memset(temp, 0, 256);
    strcpy(temp, imagePath);
    strcat(temp, "top.bmp");
    
    mShader[4].setTextureToSkyBox("U_Texture", temp);
    
    mVertexBuffer[4].setSize(4);
    mVertexBuffer[4].setPosition(0, -0.5f, 0.5f, -0.5f);
    mVertexBuffer[4].setTexcoord(0, 0.0f, 0.0f);
    mVertexBuffer[4].setPosition(1, 0.5f, 0.5f, -0.5f);
    mVertexBuffer[4].setTexcoord(1, 1.0f, 0.0f);
    mVertexBuffer[4].setPosition(2, -0.5f, 0.5f, 0.5f);
    mVertexBuffer[4].setTexcoord(2, 0.0f, 1.0f);
    mVertexBuffer[4].setPosition(3, 0.5f, 0.5f, 0.5f);
    mVertexBuffer[4].setTexcoord(3, 1.0f, 1.0f);

}

void SkyBox::initBottom(const char *imagePath){
    mShader[5].init("Res/shader/skybox.vs", "Res/shader/skybox.fs");
    char temp[256];
    memset(temp, 0, 256);
    strcpy(temp, imagePath);
    strcat(temp, "bottom.bmp");

    mShader[5].setTextureToSkyBox("U_Texture", temp);
    
    mVertexBuffer[5].setSize(4);
    mVertexBuffer[5].setPosition(0, -0.5f, -0.5f, 0.5f);
    mVertexBuffer[5].setTexcoord(0, 0.0f, 0.0f);
    mVertexBuffer[5].setPosition(1, 0.5f, -0.5f, 0.5f);
    mVertexBuffer[5].setTexcoord(1, 1.0f, 0.0f);
    mVertexBuffer[5].setPosition(2, -0.5f, -0.5f, -0.5f);
    mVertexBuffer[5].setTexcoord(2, 0.0f, 1.0f);
    mVertexBuffer[5].setPosition(3, 0.5f, -0.5f, -0.5f);
    mVertexBuffer[5].setTexcoord(3, 1.0f, 1.0f);
}

void SkyBox::draw(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix, float x, float y, float z){
    // 为了让天空盒一直处于最远处，先关闭深度测试再绘制
    glDisable(GL_DEPTH_TEST);
    mModelMatrix = glm::translate(x, y, z);
    for (int i=0; i<6; i++) {
        mVertexBuffer[i].bind();
        mShader[i].bind(glm::value_ptr(mModelMatrix), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        mVertexBuffer[i].unbind();
    }
}
