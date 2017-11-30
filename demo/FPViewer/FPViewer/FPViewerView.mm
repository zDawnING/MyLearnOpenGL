//
//  FPViewerView.m
//  FPViewer
//
//  Created by zDawnING on 17/11/22.
//  Copyright © 2017年 zDawnING. All rights reserved.
//

#import "FPViewerView.h"
#include "scene.h"
#include "utils.h"
#include <sys/time.h>

@implementation FPViewerView


/**
 使用NSData来加载文件, 实现utils中的方法

 @param path     文件相对地址
 @param fileSize 文件大小

 @return 文件内容指针
 */
unsigned char * LoadFileContent(const char *path, int &fileSize){
    unsigned char *fileContent = nullptr;
    fileSize = 0;
    
    // 通用的OS/X 或 IOS的资源管理方法, 过滤方式为空
    NSString *nsPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:path] ofType:nil];
    
    NSData *nsData = [NSData dataWithContentsOfFile:nsPath];
    
    if(nsData.length > 0){
        fileSize = [nsData length];
        fileContent = new unsigned char[fileSize + 1]; // 给指针添加多一个内存
        memcpy(fileContent, [nsData bytes], fileSize);
        fileContent[fileSize] = '\0'; //char数组后加结束标识（健壮性）
    }
    return fileContent;
}

/**
 获取每一帧花费的时间，就是两帧直接的间隔
 
 @return 间隔时间
 */
float GetFrameTime(){
    // 上一次的时间，这一次的时间
    static unsigned long long lastTime = 0, timeCurrent = 0;
    timeval current;
    gettimeofday(&current, nullptr); // 获取当前时间
    // 求出当前时间是多少毫秒(把秒转成毫秒，把微秒转成毫秒)
    timeCurrent = current.tv_sec * 1000 + current.tv_usec/1000;
    unsigned long long frameTime = lastTime == 0 ? 0 : timeCurrent - lastTime;
    lastTime = timeCurrent;
    // 最后将frameTime转成秒
    return float(frameTime) / 1000;
}

// 重载该方法并开启，才能收到键盘触发消息
-(BOOL)acceptsFirstResponder{
    return YES;
}
// 重载键盘按下和弹起事件
- (void)keyDown:(NSEvent *)event{
    onKeyDown([event.characters UTF8String][0]-32);
}
- (void)keyUp:(NSEvent *)event{
    onKeyUp([event.characters UTF8String][0]-32);
}

CGPoint originalPos, lastPos; // 用于存储鼠标按下时的点，先初始值，移动后至停止则复原
bool rotateView = false; // 是否正在旋转视角

// 重载鼠标右键的按下事件
- (void)rightMouseDown:(NSEvent *)event{
    // 创建鼠标对象
    CGEventRef ourEvent = CGEventCreate(NULL);
    // 获取鼠标全局位置
    originalPos = CGEventGetLocation(ourEvent);
    // 释放对象
    CFRelease(ourEvent);
    rotateView = true;
    // 终点初始化为按下时的起点，处理未发生拖拽情况
    lastPos = originalPos;
}
- (void)rightMouseUp:(NSEvent *)event{
    rotateView = false;
    // 鼠标复原至它按下的位置
    CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, originalPos);
}
- (void)rightMouseDragged:(NSEvent *)event{
    CGEventRef ourEvent = CGEventCreate(NULL);
    // 当鼠标发生偏移的时候重新获取点
    CGPoint point = CGEventGetLocation(ourEvent);
    CFRelease(ourEvent);
    if(rotateView){ // 只有在旋转视口的时候才进行处理
        // 当前鼠标位置-上一次鼠标位置 = 偏移
        onMouseMove(point.x - lastPos.x, point.y - lastPos.y);
        // 将历史记录设置为新的位置
        lastPos = point;
    }
}


// 重载NSOpenGLView中的方法
- (void)prepareOpenGL{
    [super prepareOpenGL];
    // 初始化场景
    init();
    // 设置视口
    setViewPortSize(800.0f, 600.0f);
    // 设置定时器
    [NSTimer scheduledTimerWithTimeInterval:0.016 target:self selector:@selector(renderOneFrame) userInfo:nil repeats:YES];
}

-(void)renderOneFrame{
    // 设置当前的视口被绘制
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    // 绘制场景
    draw();
    // 将所有指令刷新至GPU上执行
    glFlush();
}

@end
