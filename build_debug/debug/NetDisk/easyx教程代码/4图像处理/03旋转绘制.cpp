#include <graphics.h>//包含：图像库头文件
int main03 ()
{
 
	initgraph(300, 400);//创建300*400的窗口
	//按图片默认大小加载
	IMAGE img1;
	loadimage(&img1, L".\\image\\乌龟_15.png");//从硬盘任意位置加载图像，并默认图像大小 
	putimage( 0,  0, &img1);
	IMAGE imgTemp;
	/*
	角度转弧度：PI/180*角度
	弧度转角度：180/PI*弧度
	*/
	double PI = 3.1415926;
	//         旋转后     旋转前  旋转角度     空白区填色 自动大小  保真吗(高true低false)        
	rotateimage(&imgTemp, &img1, PI / 180 * 60, NULL,      true,     true);
	putimage(150, 160, &imgTemp);

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}