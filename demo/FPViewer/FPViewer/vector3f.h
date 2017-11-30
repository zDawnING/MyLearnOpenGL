//
//  vector3f.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/27.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once

#include "ggl.h"

class Vector3f{
public:
    
    float x, y, z; // 坐标值
    
    Vector3f(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    
    // 重载向量的加减
    
    Vector3f operator + (Vector3f &vec3f);
    
    Vector3f operator - (Vector3f &vec3f);
    
    // 重载标量与向量的乘法运算符
    Vector3f operator * (float scaler);
    
    // 重载点乘
    float operator * (const Vector3f &vec3f);
    
    // 重载向量赋值
    void operator = (const Vector3f &vec3f);
    
    /**
     标准化向量
     */
    void normalize();
    
    
    /**
     获取向量长度

     @return 长度值
     */
    float magnitude();
};


/**
 向量的叉乘

 @param vec1 向量1
 @param vec2 向量2

 @return 新的向量
 */
Vector3f cross(Vector3f vec1, Vector3f vec2);
