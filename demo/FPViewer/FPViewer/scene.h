//
//  scene.hpp
//  FPViewer
//
//  Created by zDawnING on 17/11/22.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#pragma once
#include "ggl.h"

void init();

void draw();

void setGrassRandom(float frameTime);

void switchDayAndNight();

void setViewPortSize(float width, float height);


void onKeyDown(char keyCode);

void onKeyUp(char keyCode);


/**
 鼠标移动方法
 
 @param deltaX 横坐标偏移
 @param deltaY 纵坐标偏移
 */
void onMouseMove(float deltaX, float deltaY);
