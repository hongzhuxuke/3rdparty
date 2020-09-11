#ifndef VHDESKTOPSHARING_H
#define VHDESKTOPSHARING_H
#include <windows.h>
/***********************************************
 * 枚举窗口
 * 非线程安装
 ***********************************************/
enum VHD_Window_Type{
    VHD_Desktop = 0x1,
    VHD_Window  = 0x2,
    VHD_AREA = 0x04,
    VHD_Desktop_And_Window =VHD_Desktop|VHD_Window
};

typedef struct VHD_WindowInfo_st{
    WCHAR name[1024];
    VHD_Window_Type type;
    HWND hwnd;
    RECT rect;
    bool isCompatible;
}VHD_WindowInfo;
void VHD_Window_Enum_init(VHD_Window_Type);
bool VHD_Window_Enum_update(VHD_WindowInfo *);
void VHD_Window_Enum_final();

//bool WindowInfoPush(VHD_WindowInfo& windowInfo);
/***********************************************
 * 区域捕获
 * 需要主线程调用
 ***********************************************/
VHD_WindowInfo VHD_CreateAreaWindow(int left, int top, int right, int bottom);

/***********************************************
 * 渲染
 ***********************************************/
void VHD_ActiveWindow(VHD_WindowInfo &info);
bool VHD_Render(VHD_WindowInfo &info,const HDC &desHdc,int width,int height,bool bRenderCursor);
#endif // VHDESKTOPSHARING_H

