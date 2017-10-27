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

  * 函数`prepareOpenGL`：初始化场景，设置定时器（60fps, 即每0.016秒执行绘制一帧的函数）
  * 函数`drawRect`: 绘制场景，把所有指令全部刷新到GPU上执行
  * 函数`renderOneFrame`: 设置当前视口被绘制

3. 新建一个场景c++文件(scene)：初始化场景，绘制场景

4. 初始化时，设置当前矩阵为投影矩阵，设置视景体（配置自定），切换当前矩阵为模型视口矩阵，添加一个单位矩阵初始化即可。

5. 绘制场景时，需要封装好glBegin(...)和glEnd(), 方便以后在实现各种图形或模型的绘制时调用。

## 基本图形实例

1. 绘制三角形示例代码：
  ``` c++
  // 生成三角形
      glBegin(GL_TRIANGLES);
      glColor4ub(255, 255, 255, 255);
      glVertex3f(-1.0f,-0.3f,-1.5f);
      glColor4ub(255, 0, 0, 255);
      glVertex3f(0.2f, -0.2f, -1.5f);
      glColor4ub(0, 255, 0, 255);
      glVertex3f(0.0f, 0.2f, -1.5f);
  
  // 生成三角形带
      glBegin(GL_TRIANGLE_STRIP);
  //设置点（正面则以逆时针进行点的无缝连接，新的绘制会覆盖旧的绘制）
      glColor4ub(255, 0, 0, 255); glVertex3f(-0.5f, -0.25f, -2.5f); // 红
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, -0.25f, -2.5f); // 蓝
      glColor4ub(0, 255, 0, 255); glVertex3f(0.0f, 0.25f, -2.5f); // 绿
      glColor4ub(255, 0, 0, 255); glVertex3f(0.5f, 0.25f, -2.5f); // 红
      glColor4ub(0, 255, 0, 255); glVertex3f(0.0f, 0.0f, -2.5f); // 绿
  
  // 生成三角扇形
      glBegin(GL_TRIANGLE_FAN);
  // 设置点（第一个点是圆心点，然后其他点每两个点形成一个三角形）
  // 公式：绘制一组相连的三角形，三角形是由第一个顶点及其后给定的顶点确定，顶点1，n+1与n+2定义了第n个三角形，共绘制了N-2个三角形
      glColor4ub(255, 0, 0, 255); glVertex3f(0.0f, 0.0f, -2.5f); // 红
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); // 蓝
      glColor4ub(0, 255, 0, 255); glVertex3f(0.4f, 0.25f, -2.5f); // 绿
      glColor4ub(0, 255, 0, 255); glVertex3f(0.2f, 0.4f, -2.5f); // 绿
      glColor4ub(0, 0, 255, 255); glVertex3f(0.0f, 0.45f, -2.5f); // 蓝
  // 提示：如果想要画出的扇形更加好看就需要注意斜率的变化计算
  
  ```
  
2. 绘制点与线段示例代码：
  ``` c++
  // 绘制点（点是像素端，是方形而不是圆形）
      glPointSize(10.0f); // (默认状态下是1.0)
      glBegin(GL_POINTS);
  // 设置点
      glColor4ub(255, 0, 0, 255); glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.25f, 0.4f, -2.5f); // 绿点
      
  // 绘制线段
      glLineWidth(10.0f);
      glBegin(GL_LINES);
  // 设置点（这里的每两个点连线之后，都不能与其他点进行连线，如果要在一个已被连线的点上延伸其他线段，必须新建一个与该坐标相同的点）
      glColor4ub(255, 0, 0, 255); glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点 （这里会有中断的痕迹）
      glColor4ub(0, 255, 0, 255); glVertex3f(0.6f, 0.4f, -2.5f); // 绿点    
  
   // 绘制连接线段
      glLineWidth(10.0f);
      glBegin(GL_LINE_STRIP);
  // 设置点（这里的第一个点是起点，后面的所有点都会相互连接）
      glColor4ub(255, 0, 0, 255);glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.6f, 0.4f, -2.5f); // 绿点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.3f, 0.5f, -2.5f); //蓝点
      
  // 绘制环状线段
      glLineWidth(10.0f);
      glBegin(GL_LINE_LOOP);
  // 设置点（这里的第一个点是起点，后面的所有点都会相互连接，最后第一个点与最后一个点会自动相连）
      glColor4ub(255, 0, 0, 255);glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.6f, 0.4f, -2.5f); // 绿点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.3f, 0.5f, -2.5f); //蓝点    
  ```
  
3. 绘制四边形示例代码：
  ``` c++
  // 绘制四边形
      glBegin(GL_QUADS);
  // 设置点（这里的每四个点形成一个四边形，一旦多余四个点则后面的点无效，另外4的倍数点也是可以生效的，每4个点一个四边形）
      glColor4ub(255, 0, 0, 255);glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.6f, 0.4f, -2.5f); // 绿点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.3f, 0.5f, -2.5f); //蓝点

      glColor4ub(255, 0, 0, 255);glVertex3f(-0.2f, -0.3f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(-0.5f, -0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(-0.6f, -0.4f, -2.5f); // 绿点
      glColor4ub(0, 0, 255, 255); glVertex3f(-0.3f, -0.5f, -2.5f); //蓝点
      
  // 绘制连接四边形
      glBegin(GL_QUAD_STRIP);
  // 设置点（每四个点中以左右每两个点连线，最终合并成一个四边形，另外每多两个点与之前四个点中的后两个点合成一个四边形，因此他们都是相互连接的）
  // 公式：绘制一组相连的的四边形。每个四边形是由一对顶点及其后给定的顶点共同确定的。顶点2n-1,2n,2n+2和2n+1定义了第n个四边形，总共绘制了n/2 - 1 个四边形
      glColor4ub(255, 0, 0, 255); glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.25f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.0f, 0.25f, -2.5f); // 绿点
      glColor4ub(255, 0, 0, 255); glVertex3f(0.25f, 0.25f, -2.5f); //红点

      glColor4ub(0, 0, 255, 255); glVertex3f(0.0f, 0.35f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.25f, 0.35f, -2.5f); // 绿点

      glColor4ub(255, 0, 0, 255); glVertex3f(0.35f, 0.5f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.4f, -2.5f); //蓝点  
  
  // 绘制多边形
      glBegin(GL_POLYGON);
  // 设置点（这里的点从三个点开始，每多n个点则为n+3边形）
      glColor4ub(255, 0, 0, 255);glVertex3f(0.0f, 0.0f, -2.5f); //红点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.5f, 0.0f, -2.5f); //蓝点
      glColor4ub(0, 255, 0, 255); glVertex3f(0.6f, 0.4f, -2.5f); // 绿点
      glColor4ub(0, 0, 255, 255); glVertex3f(0.3f, 0.5f, -2.5f); //蓝点
      glColor4ub(255, 0, 0, 255);glVertex3f(0.1f, 0.35f, -2.5f); //红点
  
  ```
4. 其实我们在一般使用场景中使用的都是使用三角形来绘制图形，因为只有三角形才能确定其三个点是共面的，其他多边形都不能确定，另一方面使用三角形进行计算在opengl中是最高效的
  
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
