#include <graphics.h>//包含：图像库头文件
int main05 ()
{
	initgraph(300, 400);//创建300*400的窗口
	//默认的坐标原点位置
	//circle(0,0,50);

	//设置新的坐标原点位置
	setorigin(150, 200);
	circle(0, 0, 50);

	//设置绘图缩放
	setlinecolor(RED);
	setaspectratio(0.5f, 0.5f);//缩放比例50%
	circle(0, 0, 50 );//同样是50的半径 比 上面的短一半

	setlinecolor(GREEN);
	setaspectratio(1.5f, 1.5f);//缩放比例150%
	circle(0, 0, 50);//同样是50的半径 比 上面的1.5倍

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}