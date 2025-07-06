#include <graphics.h>//包含：图像库头文件
int main01 ()
{
	initgraph(600, 400); 

	//设置文字颜色
	settextcolor(0x00ff00);
	
	//设置文字样式 字体 大小 粗体 斜体 下划线 删除线
	settextstyle(50, 0, L"隶书");
	outtextxy(100, 50, L"科林明伦C/C++");

	//设置字体：加粗  斜体  下划线                                  
	 settextstyle(50, 0, L"隶书",0, 0,  800,true,true,false);
	 outtextxy(100, 150, L"科林明伦C/C++");
	//                                  粗体 斜体  下划线  删除线

	//settextstyle(100, 0, L"隶书", 0, 0, 0, true, false, true);

	//在坐标位置输出文字
	//outtextxy(100, 50, L"科林明伦C/C++");

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}