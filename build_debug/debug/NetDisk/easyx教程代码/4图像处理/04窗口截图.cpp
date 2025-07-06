#include <graphics.h>//包含：图像库头文件
int main04 ()
{
 
	initgraph(300, 400);//创建300*400的窗口
	//按图片默认大小加载
	IMAGE img1;
	loadimage(&img1, L".\\image\\乌龟_15.png");//从硬盘任意位置加载图像，并默认图像大小 
	putimage(0, 0, &img1);
	
	IMAGE imgTemp;
	//窗口截图：存放地 x   y   宽   高
	getimage(&imgTemp, 30, 30, 100, 50);
	//显示查看
	putimage(200, 300, &imgTemp);

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}