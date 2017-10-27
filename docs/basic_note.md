# OpenGL基础开发流程

> 环境：Mac OSX 系统, Xcode

## 项目开发环境搭建

1. 新建一个Cocoa Project, language 选 Object-C，勾选 Use Storyboards
2. 项目配置：Build Settings -> Architectures: `i386` -> Valid Architectures: `i386 x86_64`
3. Main.storyboard: View -> select object library: OpenGL View 拖曳至View中,尺寸设定自由；设定OpenGLView: color(`32bit rgba`),depth(`24bit`)，尺寸设定自由，坐标要记得控制合适；新建一个继承`NSOpenGLView`的Cocoa class,并将.m改为.mm(方便与c++代码混编)；将OpenGLView中的Customer class改为新建的class
4. 设置好Window的尺寸，与上面的尺寸对应好。

## 绘图基础框架搭建

1. 新建头文件载入需要使用的库
  ``` c++
  #pragma once
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h> // 辅助库
  #include <OpenGL/glext.h> // GL扩展
  #include <stdio.h> // 标准C头文件
  #include <math.h>
  #include <string.h>
  #include <string>
  #include <sstream>
  #include <vector> // 容器
  #include <functional> // 函数包装器模板
  ```
2. .mm后缀文件重载NSOpenGLView中的方法：

  * 函数`prepareOpenGL`：执行包括：初始化场景，设置定时器（60fps, 即每0.016秒执行绘制一帧的函数）
  * 函数`drawRect`: 执行包括：绘制场景，把所有指令全部刷新到GPU上执行
  * 函数`renderOneFrame`: 执行包括：设置当前视口被绘制

3. 新建一个场景c++文件，执行包括：初始化场景，绘制场景

## 基本图形实例

## 自由变换图形实例

## 照相机基础

## 纹理基础

## 缓冲区基础

## 天空盒基础

## obj模型解析基础

## 地面绘制基础

## 光照基础

## 3D漫游基础

## 2D摄像机

## 粒子系统基础
