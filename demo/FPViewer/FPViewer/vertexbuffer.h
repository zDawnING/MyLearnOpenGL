//
//  vertexbuffer.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once
#include "ggl.h"

struct Vertex{
    float position[4];
    float color[4];
    float texcoord[4];
    float normal[4];
};

class VertexBuffer{
    
public:
    GLuint mVBO;
    
    Vertex *mVertexs;
    
    int mVertexCount;
    
    void setSize(int vertexCount);
    
    void setPosition(int index, float x, float y, float z, float w = 1.0);
    
    void setColor(int index, float r, float g, float b, float a = 1.0);
    
    void setTexcoord(int index, float x, float y);
    
    void setNormal(int index, float x, float y, float z);
    
    /**
     绑定当前vbo数据集
     */
    void bind();
    
    
    /**
     解绑当前vbo数据集
     */
    void unbind();
    
    /**
     获取某一个顶点的引用,用于外部修改
     
     @param index 索引
     
     @return vertexBuffer
     */
    Vertex & getVertBuffer(GLuint index);
};
