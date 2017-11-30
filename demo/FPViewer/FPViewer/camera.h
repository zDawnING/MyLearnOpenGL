//
//  camera.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/26.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once
#include "ggl.h"

class Camera{
    
public:
    
    Camera();
    
    Camera(glm::vec3 mPos, glm::vec3 mViewCenter, glm::vec3 mUp);
    
    glm::vec3 mPos, // 相机所在位置
              mViewCenter, // 看世界坐标系的那个点
              mUp; // 垂直于头顶发射出去的向量
    
    // 记录相机旋转角度
    float cameraRotate = 0.0f;
    
    // 是否开启移动相机视场
    bool openRotateView;
    
    // 是否开启自动移动相机
    bool openAutoMoveCamera;
    
    /**
     更新相机数据

     @param deltaTime 时间间隔
     */
    void update(float deltaTime);
    
    /**
     相机上下旋转，绕着自身的x轴旋转

     @param angle 旋转角度
     */
    void pitch(float angle);
    
    /**
     相机左右旋转

     @param angle 旋转角度
     */
    void yaw(float angle);
    
    /**
     旋转相机

     @param angle 角度
     @param x     x
     @param y     y
     @param z     z
     */
    void rotateView(float angle, float x, float y, float z);
};
