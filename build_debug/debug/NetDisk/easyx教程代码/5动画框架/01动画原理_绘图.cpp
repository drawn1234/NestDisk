#include <graphics.h>//包含：图像库头文件
int main01 ()
{
	initgraph(500, 400);//创建300*400的窗口

	for (int i=10;i<400;i+=10)
	{
		circle(i,100,50);//画图
		Sleep(100);//等待时间间隔 100毫秒  1秒=1000毫秒
		cleardevice();//清屏刷新
	}

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}