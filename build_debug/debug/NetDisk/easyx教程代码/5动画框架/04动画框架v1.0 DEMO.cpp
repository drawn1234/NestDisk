#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h> 

/*初始化动画*/
static void initAnime();
/*绘制动画界面*/
static void paintAnime();
/*定时处理*/
static void timer();
/*定时器时间间隔*/
static int interval = 50;
/*设置动画屏幕宽高*/
static int screenWidth = 500;
static int screenHeight = 400;

//TODO: 全局变量
static IMAGE imgs[12];
static int imgnum = 0;
int main04 ()
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
	TCHAR path[100];
	for (int i = 0; i < 12; i++)
	{
		_stprintf(path, L".\\image\\蝴蝶鱼_%d.png", i);//动态拼装图片路径
		loadimage(&imgs[i], path);//根据拼装路径加载图片到图片数组。
	}
}
//TODO: 绘制动画界面 
static void paintAnime()
{
	putimage(20, 30, &imgs[imgnum]);
}
//TODO: 定时处理 
static void timer()
{
	if (imgnum == 11)
	{
		imgnum = 0;
	}
	else
	{
		imgnum++;
	}
}