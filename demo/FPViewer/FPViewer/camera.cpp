//
//  camera.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/26.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "camera.h"
#include "vector3f.h"

Camera::Camera(){
    mPos = glm::vec3(0.0f, 0.0f, 0.0f);
    mViewCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

Camera::Camera(glm::vec3 mPos, glm::vec3 mViewCenter, glm::vec3 mUp){
    this->mPos = mPos;
    this->mViewCenter = mViewCenter;
    this->mUp = mUp;
    // 默认关闭手动更新相机数据
    openRotateView = false;
    // 默认开启相机自动移动
    openAutoMoveCamera = true;
}

/**
 更新相机位置
 
 @param deltaTime 时间间隔
 */
void Camera::update(float deltaTime){
    
}

/**
 相机上下旋转，绕着自身的x轴旋转
 
 @param angle 旋转角度
 */
void Camera::pitch(float angle){
    Vector3f tempViewCenter(mViewCenter.x, mViewCenter.y, mViewCenter.z);
    Vector3f tempPos(mPos.x, mPos.y, mPos.z);
    Vector3f tempUp(mUp.x, mUp.y, mUp.z);
    Vector3f viewDirection = tempViewCenter - tempPos; // 获取当前视线方向
    viewDirection.normalize(); // 单位化，转换为基向量
    Vector3f rightDirection = cross(viewDirection, tempUp); // 指向右边是要遵循叉乘的右手法则
    rightDirection.normalize(); // 单位化
    rotateView(angle, rightDirection.x, rightDirection.y, rightDirection.z);
}

/**
 相机左右旋转
 
 @param angle 旋转角度
 */
void Camera::yaw(float angle){
    // 由于相机是，那么直接沿着mUp进行旋转即可
    rotateView(angle, mUp.x, mUp.y, mUp.z);
//    std::cout << "camera:" << angle << std::endl;
}

/**
 旋转相机
 
 @param angle 角度
 @param x     x
 @param y     y
 @param z     z
 */
void Camera::rotateView(float angle, float x, float y, float z){
    
    Vector3f tempViewCenter(mViewCenter.x, mViewCenter.y, mViewCenter.z);
    Vector3f tempPos(mPos.x, mPos.y, mPos.z);
    Vector3f viewDirection = tempViewCenter - tempPos; // 视线向量
    Vector3f newDirection(0.0f, 0.0f, 0.0f);
    float C = cosf(angle);
    float S = sinf(angle);
    Vector3f tempX(C + x*x*(1 - C), x*y*(1 - C) - z*S, x*z*(1 - C) + y*S);
    newDirection.x = tempX*viewDirection;
    Vector3f tempY(x*y*(1 - C) + z*S, C + y*y*(1 - C), y*z*(1 - C) - x*S);
    newDirection.y = tempY*viewDirection;
    Vector3f tempZ(x*z*(1 - C) - y*S, y*z*(1 - C) + x*S, C + z*z*(1 - C));
    newDirection.z = tempZ*viewDirection;
    // 此时新的视点为当前位置加上新的方向向量
    tempViewCenter = tempPos + newDirection;
    mViewCenter = glm::vec3(tempViewCenter.x, tempViewCenter.y, tempViewCenter.z);
    
}

