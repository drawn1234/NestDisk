#include <graphics.h>//包含：图像库头文件
int main01 ()
{
	initgraph(300, 400);//创建300*400的窗口
	//设置颜色
	setlinecolor(0xff0000);
	//设置样式(多选一) 和 粗细
	//setlinestyle(PS_SOLID,3);//实线
	//setlinestyle(PS_DASH,3);//虚线
	//setlinestyle(PS_DOT,3);//点线
	//setlinestyle(PS_DASHDOT,3);//虚 点线
	setlinestyle(PS_DASHDOTDOT ,3);//虚 点点 线
	//画线：两点确定一条直线
	line(50, 100, 200, 100);



	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}