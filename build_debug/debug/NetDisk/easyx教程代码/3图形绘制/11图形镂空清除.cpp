#include <graphics.h>//包含：图像库头文件
int main11 ()
{
	initgraph(300, 400);//创建300*400的窗口
	setbkcolor(RED);//设置背景色
	cleardevice();//刷新窗口
	/*绘制一个图像*/
	IMAGE imgBK  ;
	loadimage(&imgBK, L".\\image\\背景_03.png");
	putimage(0, 0, &imgBK);
	//镂空一个圆形
	clearcircle(80, 80, 50);
	int w = 50, h = 80;
	//镂空一个椭圆形
	clearellipse(40, 100, 40 + w, 100 + h);
	//镂空一个矩形
	clearrectangle(40, 100, 40 + w, 100 + h);
	//镂空一个圆角矩形
	clearroundrect(40, 100, 40 + w, 100 + h, 10, 10);
	 
	double PI = 3.1415926;
	//镂空一个扇形
	clearpie(40, 100, 40 + w, 100 + h, PI / 180 * 0, PI / 180 * 90);
	//镂空一个三角形
	POINT pts[] = { {50, 200}, {200, 200}, {200, 50} };
	clearpolygon(pts, 3);
	
	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}