#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>//包含：图像库头文件
int main ()
{
	initgraph(600, 400); 

	//设置文字颜色
	settextcolor(0x00ff00);
	//设置文字样式 字体 大小 粗体 斜体 下划线 删除线
	settextstyle(50, 0, L"隶书");
	//                                  粗体 
	//settextstyle(100, 0, L"隶书",0, 0,  800,false,false,false);
	//                                  粗体 斜体  下划线  删除线
	//settextstyle(100, 0, L"隶书", 0, 0, 0, true, false, true);
 
	TCHAR str[100];
	for (int s=10;s>=0;s--)
	{
		_stprintf(str, L"倒计时：%d",s);
		outtextxy(100, 50, str);
		Sleep(1000);
		cleardevice();
	}
 
	RGB(45, 50, 60);
	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}