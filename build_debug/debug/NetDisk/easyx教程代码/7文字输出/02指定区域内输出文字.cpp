#include <graphics.h>//包含：图像库头文件
int main02  ()
{
	initgraph(600, 400); 
	setbkmode(TRANSPARENT);
	//设置文字颜色
	settextcolor(0x00ff00);
	//设置文字样式 字体 大小 粗体 斜体 下划线 删除线
	settextstyle(30, 0, L"隶书");
	//                                  粗体 
	//settextstyle(100, 0, L"隶书",0, 0,  800,false,false,false);
	//                                  粗体 斜体  下划线  删除线
	//settextstyle(100, 0, L"隶书", 0, 0, 0, true, false, true);

	//在区域内设置文字
	//
	rectangle(100, 50, 100 + 400, 50 + 80);
	RECT r = { 100, 50, 100+400, 50+80 };
	//drawtext(L"Hello World", &r,DT_RIGHT);
	drawtext(L"Hello  World " , &r, DT_CENTER | DT_VCENTER |DT_SINGLELINE );
	 


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}