//
//  model.cpp
//  FPViewer
//
//  Created by zDawnING on 17/11/24.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#include "model.h"
#include "utils.h"
#include "vector3f.h"

using namespace std;

Model::Model(){
    mMoveLeft = false;
    mMoveRight = false;
    mMoveForward = false;
    mMoveBack = false;
    
    openKeyDownMove = false;
    
    scaleX = 1.0f;
    scaleY = 1.0f;
    
    worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
};

void Model::init(const char *modelPath){
    // 表示一组三个分量的数组，用于存储obj中某一行的3个浮点数或者2个
    struct FloatData{
        float vec3f[3];
    };
    // 定义vertex索引块
    struct VertexDefine{
        int posIndex;
        int texcoordIndex;
        int normalIndex;
    };
    
    int fileSize = 0;
    unsigned char *fileContent = LoadFileContent(modelPath, fileSize);
    if(fileContent == nullptr){
        return;
    }
    
    vector<FloatData> positions, texcoords, normals;
    vector<VertexDefine> vertexes;
    string temp; // 解码时用于辅助的变量
    stringstream ssFileContent((char*)fileContent);
    char szOneLine[256]; // 行内存记录
    
    while (!ssFileContent.eof()) {
        // 初始化
        memset(szOneLine, 0, 256);
        ssFileContent.getline(szOneLine, 256);
        if(strlen(szOneLine) > 0){
            if(szOneLine[0] == 'v'){
                // 将某一行的数据输入到流中,流中会根据空格来分隔输出
                stringstream ssOneLine(szOneLine);
                if(szOneLine[1] == 't'){ // 纹理坐标
                    ssOneLine >> temp;
                    FloatData data;
                    ssOneLine >> data.vec3f[0];
                    ssOneLine >> data.vec3f[1];
                    texcoords.push_back(data);
                }else if(szOneLine[1] == 'n'){
                    ssOneLine >> temp;
                    FloatData data;
                    ssOneLine >> data.vec3f[0];
                    ssOneLine >> data.vec3f[1];
                    ssOneLine >> data.vec3f[2];
                    normals.push_back(data);
                }else{
                    ssOneLine >> temp;
                    FloatData data;
                    ssOneLine >> data.vec3f[0];
                    ssOneLine >> data.vec3f[1];
                    ssOneLine >> data.vec3f[2];
                    positions.push_back(data);
                }
            }
            if(szOneLine[0] == 'f'){
                stringstream ssOneLine(szOneLine);
                ssOneLine >> temp;
                string vertexStr;
                for (int i=0; i<3; i++) {
                    ssOneLine >> vertexStr;
                    size_t pos = vertexStr.find_first_of('/');
                    string posIndexStr = vertexStr.substr(0, pos);
                    size_t pos2 = vertexStr.find_first_of('/', pos+1);
                    string texcoordIndexStr = vertexStr.substr(pos+1, pos2 - 1 - pos);
                    string normalIndexStr = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);
                    VertexDefine vd;
                    vd.posIndex = atoi(posIndexStr.c_str());
                    vd.texcoordIndex = atoi(texcoordIndexStr.c_str());
                    vd.normalIndex = atoi(normalIndexStr.c_str());
                    vertexes.push_back(vd);
                }
            }
        }
    }
    delete fileContent;
    
    // 设置vertexBuffer
    int vertCount = (int)vertexes.size();
    mVertexBuffer = new VertexBuffer;
    mVertexBuffer->setSize(vertCount);
    for (int i=0; i<vertCount; i++) {
        // 这里要注意obj模型中索引下标是从1开始的
        float *temp = positions[vertexes[i].posIndex - 1].vec3f;
        mVertexBuffer->setPosition(i, temp[0], temp[1], temp[2]);
        if(!normals.empty()){
            temp = normals[vertexes[i].normalIndex - 1].vec3f;
            mVertexBuffer->setNormal(i, temp[0], temp[1], temp[2]);
        }
        if(!texcoords.empty()){
            temp = texcoords[vertexes[i].texcoordIndex - 1].vec3f;
            mVertexBuffer->setTexcoord(i, temp[0], temp[1]);
        }
    }
    
    // 设置shader
    mShader = new Shader;
    mShader->init("Res/shader/model.vs", "Res/shader/model.fs");
    
    // 设置光分量
    mShader->setVec4f("U_LightPos", 0.0f, 1.0f, 1.0f, 0.0f); // 设置光源位置
    mShader->setVec4f("U_LightAmbient", 0.3f, 0.3f, 0.3f, 1.0f); // 设置环境光分量
    mShader->setVec4f("U_LightDiffuse", 1.0f, 1.0f, 1.0f, 1.0f); // 设置漫反射光分量
    mShader->setVec4f("U_LightSpecular", 1.0f, 1.0f, 1.0f, 1.0f); // 设置镜面反射光分量
    
    // 设置材质对光的反射系数
    setAmbientMaterial(0.1f, 0.1f, 0.1f, 1.0f);
    setDiffuseMaterial(0.6f, 0.6f, 0.6f, 1.0f);
    setSpecularMaterial(1.0f, 1.0f, 1.0f, 1.0f);
    
    mShader->setVec4f("U_CameraPos", 0.0f, 0.0f, 0.0f, 1.0f); // 照相机位置
    mShader->setVec4f("U_LightOpt", 32.0f, 0.0f, 0.0f, 0.0f); // 设置32次幂
}

void Model::draw(glm::mat4 &viewMatrix, glm::mat4 projectionMatrix, float x, float y, float z){
    glEnable(GL_DEPTH_TEST);
    
    mVertexBuffer->bind();
    mShader->bind(glm::value_ptr(mModelMatrix), glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix));
    glDrawArrays(GL_TRIANGLES, 0, mVertexBuffer->mVertexCount);
    mVertexBuffer->unbind();
}

void Model::update(float frameTime){
    
}

void Model::setPosition(float x, float y, float z){
    mModelMatrix = glm::translate(x, y, z);
    worldPos = glm::vec3(x, y, z);
}

void Model::setScale(float x, float y, float z){
    scaleX = x;
    scaleY = y;
    scaleZ = z;
    mModelMatrix = glm::scale(x, y, z);
}

void Model::setRotate(float angle){
    float tempAngle = angle * (180.0f / M_PI);
    mModelMatrix *= glm::rotate(tempAngle, 0.0f, 1.0f, 0.0f);
//    Vector3f tempDir(0.0f, 0.0f, 0.0f);
//    tempDir.x = cosf(tempAngle * (M_PI / 180.0f));
//    tempDir.y = 1.0f;
//    tempDir.z = sinf(tempAngle * (M_PI / 180.0f));
//    tempDir.normalize();
//    viewDirection = glm::vec3(tempDir.x, tempDir.y, tempDir.z);
}

void Model::movePosition(float deltaTime){
    glm::mat4 identityMatrix;
    if(openKeyDownMove){
        float moveSpeed = 10.0f;
        // 带旋转后，沿轴方向移动已经不合适了
        if(mMoveLeft){ // 左移则是x坐标的负方向
            float delta = moveSpeed * deltaTime;
            worldPos.x -= delta;
//            setPosition(worldPos.x, worldPos.y, worldPos.z);
            
        }
        if(mMoveRight){
            float delta = moveSpeed * deltaTime;
            worldPos.x += delta;
            
        }
        if(mMoveForward){
            float delta = moveSpeed * deltaTime;
            worldPos.z -= delta;
            
        }
        if(mMoveBack){
            float delta = moveSpeed * deltaTime;
            worldPos.z += delta;
            
        }
        mModelMatrix = identityMatrix;
        mModelMatrix *= glm::translate(worldPos.x, worldPos.y, worldPos.z);
        mModelMatrix *= glm::scale(scaleX, scaleY, scaleZ);
        
//        Vector3f tempPos(worldPos.x, worldPos.y, worldPos.z);
//        Vector3f tempView(viewDirection.x, viewDirection.y, viewDirection.z);
//        Vector3f tempUp(0.0f, 1.0f, 0.0f);
//        
//        Vector3f tempRight = cross(tempView, tempUp);
//        tempRight.normalize();
//        
//        if(mMoveLeft){ // 左移则是x坐标的负方向
//            Vector3f delta = tempRight * moveSpeed * deltaTime;
//            tempPos = tempPos - delta;
//        }
//        if(mMoveRight){
//            Vector3f delta = tempRight * moveSpeed * deltaTime;
//            tempPos = tempPos + delta;
//        }
//        if(mMoveForward){
//            Vector3f delta = tempView * moveSpeed * deltaTime;
//            tempPos = tempPos - delta;
//        }
//        if(mMoveBack){
//            Vector3f delta = tempView * moveSpeed * deltaTime;
//            tempPos = tempPos + delta;
//        }
    }
}

void Model::setAmbientMaterial(float r, float g, float b, float a){
    mShader->setVec4f("U_AmbientMaterial", r, g, b, a);
}

void Model::setDiffuseMaterial(float r, float g, float b, float a){
    mShader->setVec4f("U_DiffuseMaterial", r, g, b, a);
}

void Model::setSpecularMaterial(float r, float g, float b, float a){
    mShader->setVec4f("U_SpecularMaterial", r, g, b, a);
}

void Model::setTexture(const char *imagePath){
    mShader->setTexture("U_Texture", imagePath);
}

void Model::setTexture(const char *name, const char *imagePath){
    mShader->setTexture(name, imagePath);
}

