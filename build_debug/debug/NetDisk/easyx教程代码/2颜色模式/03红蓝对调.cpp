#include <graphics.h>//包含：图像库头文件
#include <stdio.h>
int main03()
{
	initgraph(300, 400, EW_SHOWCONSOLE);//创建300*400的窗口

	//得到颜色对应值
	setbkcolor(BLUE);
	cleardevice();

	for (int i=0;i<50;i++)
	{
		Sleep(300);
		setbkcolor(BGR(getbkcolor()));//把背景色进行红蓝对调
		cleardevice();
	}

 

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}