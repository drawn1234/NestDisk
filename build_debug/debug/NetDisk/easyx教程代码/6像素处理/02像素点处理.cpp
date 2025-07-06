#include <graphics.h>//包含：图像库头文件
#include <stdio.h>
int main()
{
	initgraph(600, 400 );//创建300*400的窗口
	IMAGE bk;
	loadimage(&bk, L".\\image\\背景_03.png",600,400,true);
	putimage(0, 0, &bk);
	//设置一个像素点的颜色
	/*putpixel(100, 100, 0xffffff);
	putpixel(100, 105, 0x00ff00);
	putpixel(105, 100, 0x0000ff);*/

	for (int x=0;x<600;x++)
	{
		for (int y=0;y<400;y++)
		{
			COLORREF color = getpixel(x, y);
			color ^= 0x00ff00;//对每个像素点颜色做和绿色的异或操作。
			putpixel(x, y, color);
		}
	}


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}