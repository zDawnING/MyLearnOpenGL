#include "vector3f.h"

Vector3f::Vector3f(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

// 重载向量的加减

Vector3f Vector3f::operator + (Vector3f &vec3f){
    return Vector3f(x + vec3f.x, y + vec3f.y, z + vec3f.z);
}

Vector3f Vector3f::operator - (Vector3f &vec3f){
    return Vector3f(x - vec3f.x, y - vec3f.y, z + vec3f.z);
}

// 重载标量与向量的乘法运算符
Vector3f Vector3f::operator * (float scaler){
    return Vector3f(x * scaler, y * scaler, z * scaler);
}

// 重载点乘
float Vector3f::operator * (const Vector3f &vec3f){
    return x * vec3f.x + y * vec3f.y + z * vec3f.z;
}

// 重载向量赋值
void Vector3f::operator = (const Vector3f &vec3f){
    x = vec3f.x;
    y = vec3f.y;
    z = vec3f.z;
}

/**
 标准化向量
 */
void Vector3f::normalize(){
    float len = magnitude();
    x /= len;
    y /= len;
    z /= len;
}


/**
 获取向量长度
 
 @return 长度值
 */
float Vector3f::magnitude(){
    return sqrtf(x*x + y*y + z*z);
}

/**
 向量的叉乘
 
 @param vec1 向量1
 @param vec2 向量2
 
 @return 新的向量
 */
Vector3f cross(Vector3f vec1, Vector3f vec2){
    return Vector3f(vec1.y*vec2.z - vec1.z*vec2.y, vec1.z*vec2.x - vec1.x*vec2.z, vec1.x*vec2.y - vec1.y*vec2.x);
}
