#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>//包含：图像库头文件

/*初始化动画*/
static void initAnime();
/*绘制动画界面*/
static void paintAnime();
/*定时处理*/
static void timer();
/*定时器时间间隔*/
static int interval = 100;
/*设置动画屏幕宽高*/
static int screenWidth = 500;
static int screenHeight = 400;

//TODO: 全局变量


 int main03 ()
{
	initgraph(screenWidth, screenHeight);//创建300*400的窗口
	initAnime();//初始化加载图片
	while (1)
	{
		BeginBatchDraw();//开启批量绘图
		cleardevice();//清空画板
		paintAnime();//动画绘图
		EndBatchDraw();//结束批量绘图

		timer();//动做逻辑

		Sleep(interval);//动画帧时间间隔
	}
	

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}

//TODO: 初始化动画 
 static void initAnime()
{
	 
}
//TODO: 绘制动画界面 
 static void paintAnime()
{
	 
}
//TODO: 定时处理 
 static void timer()
{
	 
}