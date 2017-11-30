//
//  scene.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/22.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "scene.h"
#include "utils.h"
#include "ground.h"
#include "skybox.h"
#include "model.h"
#include "camera.h"
#include "particlesystem.h"

// 初始相机位置
glm::vec3 cameraPos(-10.0f, 6.0f, 0.0f);

// 添加相机对象
Camera camera;

// 添加三个矩阵
glm::mat4 modelMatrix, viewMatrix, projectionMatrix;

// 地面对象
Ground ground;

// 天空盒对象
SkyBox skybox;

// 模型对象
Model niutou, grass, tree;

// 粒子系统对象
ParticleSystem particleSys;


// 用于记录间隔秒数
static float deltaTime = 0.0f;

// 记录相机旋转角度
float cameraRotate = 0.0f;

// 设置夜间模式
bool isNight = true;

/**
 圆形路轨旋转相机
 */
void rotateCameraRail(){
    if(cameraRotate >= 360.0f){
        cameraRotate = 0.0f;
    }
    cameraRotate += 0.5f;
    cameraPos.x = 10.0f * cosf(cameraRotate * (M_PI / 180.0f));
    cameraPos.y = 7.0f + 3.0f * cosf(cameraRotate * (M_PI / 180.0f)); // 附加cos函数曲线
    cameraPos.z = 10.0f * sinf(cameraRotate * (M_PI / 180.0f));
    viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void autoMoveCamera(float frameTime){
    rotateCameraRail();
}


/**
 随机设置草的位置

 @param frameTime 每帧的秒数
 */
void setGrassRandom(float frameTime){
    deltaTime += frameTime;
    if(deltaTime > 10.0f){
        float numX = 0.0f;
        float numZ = 0.0f;
        srand((unsigned) time(NULL));
        numX = rand() % 30;
        numZ = rand() % 30;
        grass.setPosition(numX, 0.0f, numZ);
        deltaTime = 0.0f;
    }
}


/**
 选择白天或黑夜
 */
void switchDayAndNight(){
    skybox.init("Res/night/");
}

void onKeyDown(char keyCode){
    switch (keyCode) {
        case 'W':
            niutou.mMoveForward = true;
            break;
        case 'S':
            niutou.mMoveBack = true;
            break;
        case 'A':
            niutou.mMoveLeft = true;
            break;
        case 'D':
            niutou.mMoveRight = true;
            break;
    }
}
void onKeyUp(char keyCode){
    switch (keyCode) {
        case 'W':
            niutou.mMoveForward = false;
            break;
        case 'S':
            niutou.mMoveBack = false;
            break;
        case 'A':
            niutou.mMoveLeft = false;
            break;
        case 'D':
            niutou.mMoveRight = false;
            break;
    }
}
void onMouseMove(float deltaX, float deltaY){
    if(camera.openRotateView){
        float angleRotatedByUp = deltaY / 1000.0f;
        float angleRotatedByRight = deltaX / 1000.0f;
        camera.pitch(-angleRotatedByUp);
        camera.yaw(-angleRotatedByRight);
        viewMatrix = glm::lookAt(camera.mPos, camera.mViewCenter, camera.mUp);
        // 相机的移动要控制物体的移动
        niutou.setRotate(-angleRotatedByRight);
    }
}

void emitParticle(){
//    static float currentSleepTime = 0.0f; //当前睡眠了多少秒
//    static float nextWaitTime = 0.016f; // 发射下一颗粒子
//    static int particleCount = 1; // 记录当前已经发射了多少粒子，默认已发射一颗
//    if(particleCount == 1000){
//        return;
//    }
//    currentSleepTime += deltaTime; // 开启计时
//    if(currentSleepTime >= nextWaitTime){ // 如果当前睡眠时间刚好到达一个间隔时间，立即初始化为0，否则不执行
//        currentSleepTime = 0.0f;
//    }else{
//        return;
//    }
//    particles[particleCount - 1].mHalfSize = 30.0f;
//    particles[particleCount - 1].mTexture = particleTexture;
//    // 传入颜色和外带生命值
//    particles[particleCount - 1].Init(220, 150, 50, 255, 10.0f);
//    particleCount++;
}

void init(){
    
    // 初始化相机位置
    camera = *new Camera(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    
    // 设置viewMatrix
    viewMatrix = glm::lookAt(camera.mPos, camera.mViewCenter, camera.mUp);
    
    camera.openRotateView = false;
    
    // 初始化地面
    ground.init(isNight);
    ground.setTexture("Res/day/grass2.JPG");
    
    // 初始化牛头人
    niutou.init("Res/obj/ntr/niutou.obj");
    niutou.setTexture("Res/obj/ntr/niutou.bmp");
//    niutou.setScale(0.03f, 0.03f, 0.03f);
//    niutou.setPosition(-5.0f, -1.0f, 0.0f);
    
    niutou.mModelMatrix = glm::translate(-5.0f, -1.0f, 0.0f) * glm::scale(0.03f, 0.03f, 0.03f);
//    niutou.mModelMatrix = glm::scale(0.03f, 0.03f, 0.03f);
    
    // 初始化草
    grass.init("Res/obj/grass/grass.obj");
    grass.setTexture("Res/obj/grass/Grass.png");
    grass.setPosition(0.0f, -1.0f, 0.0f);
    
    // 初始化天空盒
    skybox.init("Res/night/");
    
    // 初始化粒子系统
    particleSys.init(0.0f, 1.5f, 0.0f);
}

void draw(){
    float frameTime = GetFrameTime(); // 根据间隔时间绘制
    
    glClearColor(0.1f, 0.4f, 0.6f, 1.0f);
    
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 开启相机自动旋转
    autoMoveCamera(frameTime);
    
    // 先绘制天空盒
    skybox.draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
    
    ground.draw(viewMatrix, projectionMatrix);
    
    niutou.openKeyDownMove = false;
//    niutou.movePosition(frameTime);
    
    niutou.draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
    
//    setGrassRandom(frameTime);
    
    grass.draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
    
    particleSys.update(frameTime);
    
    particleSys.draw(viewMatrix, projectionMatrix);
    
//    emitPraticle(frameTime);
//    // 绘制发射多粒子器
//    for (int i=0; i<1000; i++) {
//        // 默认识别该粒子是否为-1，不为-1则表示粒子已经被发射出去，否则仍未发射
//        if(particles[i].mLifeTime != -1){
//            particles[i].update(frameTime);
//            particles[i].Draw();
//        }else{
//            return;
//        }
//    }
    
}

void setViewPortSize(float width, float height){
    projectionMatrix = glm::perspective(60.0f, width/height, 0.1f, 1000.0f);
}
