#include <graphics.h>//包含：图像库头文件
int main05 ()
{
	initgraph(300, 400);//创建300*400的窗口
	//设置颜色
	setlinecolor(0x0000ff);
	//设置样式(多选一) 和 粗细
	  setlinestyle(PS_SOLID,1);//实线
	//setlinestyle(PS_DASH,3);//虚线
	//setlinestyle(PS_DOT,3);//点线
	//setlinestyle(PS_DASHDOT,3);//虚 点线
	//setlinestyle(PS_DASHDOTDOT, 1);//虚 点点 线
	 //绘制空心圆角矩形
	int w = 50, h = 80;
	roundrect(40,100, 40+w, 100+h,10,10);

	//设置 填充颜色
	setfillcolor(0x00ff00);//绿色
	//设置填充样式
	//1 不填充
	//setfillstyle(BS_NULL);
	//fillroundrect(40,100, 40+w, 100+h, 10, 10);
	
	//2 实心填充
	 
	//setfillstyle(BS_SOLID);
	//solidroundrect(100, 100,100+w , 100+h, 10, 10);
	//fillroundrect(180, 100, 180+w, 100+h, 10, 10);
	 

	//3 图案填充
	//setfillstyle(BS_HATCHED, HS_HORIZONTAL);//水平线填充
	//setfillstyle(BS_HATCHED, HS_VERTICAL);//垂直线填充
	//setfillstyle(BS_HATCHED, HS_FDIAGONAL);//右斜线填充
	//setfillstyle(BS_HATCHED, HS_BDIAGONAL);//左斜线填充
	//setfillstyle(BS_HATCHED, HS_CROSS);//方格线填充
	//setfillstyle(BS_HATCHED, HS_DIAGCROSS);//斜网格线填充
	//solidroundrect(100, 100, 100 + w, 100 + h, 10, 10);
	//fillroundrect(180, 100, 180 + w, 100 + h, 10, 10);

	//3 图像填充
	//IMAGE img;//定义图片存储变量
	//loadimage(&img, L".\\image\\0.png", 20, 20, true);//加载图片，并大小缩放
	//setfillstyle(BS_DIBPATTERN, NULL,&img);
	//solidroundrect(100, 100,100+w , 100+h, 10, 10);
	//fillroundrect(180, 100, 180+w, 100+h, 10, 10);


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}