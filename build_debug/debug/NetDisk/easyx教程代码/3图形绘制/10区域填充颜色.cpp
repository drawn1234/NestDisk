#include <graphics.h>//包含：图像库头文件
int main10 ()
{
	initgraph(300, 400);//创建300*400的窗口
	//设置颜色
	setlinecolor(0xff0000);
	//设置样式(多选一) 和 粗细
	setlinestyle(PS_SOLID,1);//实线
	//setlinestyle(PS_DASH,3);//虚线
	//setlinestyle(PS_DOT,3);//点线
	//setlinestyle(PS_DASHDOT,3);//虚 点线
	//setlinestyle(PS_DASHDOTDOT, 3);//虚 点点 线
	circle(100, 70, 50);
	circle(60, 70, 50);
	//封闭区域内填充
	setfillcolor(0x00ff00);//绿色
	//区域内填充 
	//       位置点xy 区域边界颜色 封闭区域内填充
	floodfill(70, 70, 0xff0000, FLOODFILLBORDER);

	//// 区域内符合颜色表面填充
	//circle(60, 70, 50);
	//setfillcolor(0x00ff99);
	//solidcircle(60, 70, 50);
	//setfillcolor(0x0000ff);// 
	//floodfill(70, 70, 0x00ff99, FLOODFILLSURFACE);

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}