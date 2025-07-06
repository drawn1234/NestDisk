#include <graphics.h> 
#include <stdio.h>
int main00  ()
{
	initgraph(300, 400,EW_SHOWCONSOLE);//创建300*400的窗口
	 
	printf("窗口绘图区的宽 %d 窗口的高 %d",
		getwidth(),getheight());

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}