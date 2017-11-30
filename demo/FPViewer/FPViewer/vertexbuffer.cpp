//
//  vertexbuffer.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "vertexbuffer.h"
#include "utils.h"

void VertexBuffer::setSize(int vertexCount){
    mVertexCount = vertexCount;
    mVertexs = new Vertex[mVertexCount];
    // 初始化
    memset(mVertexs, 0, sizeof(Vertex) * mVertexCount);
    
    // 在设置大小的时候边将mVBO创建出来，数据传空，此时glBufferData会在显卡上开辟一片区域而不赋值,因此该方法具备开辟空间和赋值操作
    mVBO = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertexCount, GL_STATIC_DRAW, nullptr);
}

void VertexBuffer::setPosition(int index, float x, float y, float z, float w){
    mVertexs[index].position[0] = x;
    mVertexs[index].position[1] = y;
    mVertexs[index].position[2] = z;
    mVertexs[index].position[3] = w;
}

void VertexBuffer::setColor(int index, float r, float g, float b, float a){
    mVertexs[index].color[0] = r;
    mVertexs[index].color[1] = g;
    mVertexs[index].color[2] = b;
    mVertexs[index].color[3] = a;
}

void VertexBuffer::setTexcoord(int index, float x, float y){
    mVertexs[index].texcoord[0] = x;
    mVertexs[index].texcoord[1] = y;
}

void VertexBuffer::setNormal(int index, float x, float y, float z){
    mVertexs[index].normal[0] = x;
    mVertexs[index].normal[1] = y;
    mVertexs[index].normal[2] = z;
}

/**
 绑定当前vbo数据集
 */
void VertexBuffer::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // 此处将CPU中的数据一次性全部拷贝至GPU，效率上并不高效
    // submitData缩写，参数2是上传至GPU的0号偏移扇区，即vbo的0号偏移扇区
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*mVertexCount, mVertexs);
}


/**
 解绑当前vbo数据集
 */
void VertexBuffer::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 获取某一个顶点的引用,用于外部修改
 
 @param index 索引
 
 @return vertexBuffer
 */
Vertex & VertexBuffer::getVertBuffer(GLuint index){
    return mVertexs[index];
}
