#include <graphics.h>//包含：图像库头文件
int main06   ()
{
	initgraph(300, 400);//创建300*400的窗口
	//设置颜色
	setlinecolor(0x0000ff);
	//设置样式(多选一) 和 粗细
	//setlinestyle(PS_SOLID,3);//实线
	//setlinestyle(PS_DASH,3);//虚线
	//setlinestyle(PS_DOT,3);//点线
	//setlinestyle(PS_DASHDOT,3);//虚 点线
	setlinestyle(PS_DASHDOTDOT, 3);//虚 点点 线
	 //绘制 圆弧形
	/*
	角度转弧度：PI/180*角度
弧度转角度：180/PI*弧度
*/
	double PI = 3.1415926;
	int w = 100, h = 100;
		//x y      宽      高     开始角         结束角					
	arc(40, 100, 40 + w, 100 + h, PI / 180*0, PI / 180 * 90);
	arc(40, 100, 40 + w, 100 + h, PI / 180 * 180, PI / 180 * 270);
	 
	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}