#include <graphics.h>  
#include <stdio.h>
int main04 ()
{
	initgraph(300, 400, EW_SHOWCONSOLE);//显示控制台
	Sleep(3000);//3秒后改变标题栏
	// 获得窗口句柄
	HWND hWnd = GetHWnd();
	// 使用 Windows API 修改窗口名称
	SetWindowText(hWnd, L"Hello!");
	 
	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}