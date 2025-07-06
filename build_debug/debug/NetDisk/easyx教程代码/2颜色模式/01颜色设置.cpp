#include <graphics.h>//包含：图像库头文件
int main ()
{
	initgraph(300, 400);//创建300*400的窗口
	/*
	一、预定义颜色
	常量			值			颜色
	--------		--------	--------
	BLACK			0			黑
	BLUE			0xAA0000	蓝
	GREEN			0x00AA00	绿
	CYAN			0xAAAA00	青
	RED				0x0000AA	红
	MAGENTA			0xAA00AA	紫
	BROWN			0x0055AA	棕
	LIGHTGRAY		0xAAAAAA	浅灰
	DARKGRAY		0x555555	深灰
	LIGHTBLUE		0xFF5555	亮蓝
	LIGHTGREEN		0x55FF55	亮绿
	LIGHTCYAN		0xFFFF55	亮青
	LIGHTRED		0x5555FF	亮红
	LIGHTMAGENTA	0xFF55FF	亮紫
	YELLOW			0x55FFFF	黄
	WHITE			0xFFFFFF	白
	*/
	setbkcolor( BLUE);
	cleardevice();//清除刷新界面
	Sleep(2000);


	/*二、 十六进制模式 B蓝色G绿色R红色*/
	setbkcolor(0x00ff00);
	cleardevice();//清除刷新界面
	Sleep(2000);


	/*三、 红绿蓝 十进制*/
	COLORREF color = RGB(255, 0, 0);
	setbkcolor(color);
	cleardevice();//清除刷新界面



	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}