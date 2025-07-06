#include <graphics.h>//包含：图像库头文件
#include <stdio.h>
int main02()
{
	initgraph(300, 400,EW_SHOWCONSOLE);//创建300*400的窗口

	//得到颜色对应值
	setbkcolor(BLUE);
	cleardevice();
	//分别取出对应的RGB值
	printf("红色值：%d 绿色值：%d 蓝色值：%d ",
		GetRValue(BLUE), GetGValue(BLUE), GetBValue(BLUE)  );



	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}