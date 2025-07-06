#include <graphics.h>//包含：图像库头文件
int main04  ()
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
	 //绘制空心矩形
	int w = 50, h = 80;
	 rectangle(40,100, 40+w, 100+h);

	//设置 填充颜色
	setfillcolor(0x00ff00);//绿色
	//设置填充样式
	//1 不填充
	//setfillstyle(BS_NULL);
	//fillrectangle(40,100, 40+w, 100+h);
	
	//2 实心填充
	 
	/*setfillstyle(BS_SOLID);
	solidrectangle(100, 100,100+w , 100+h);
	fillrectangle(180, 100, 180+w, 100+h );*/
	 

	//3 图案填充
	//setfillstyle(BS_HATCHED, HS_HORIZONTAL);//水平线填充
	//setfillstyle(BS_HATCHED, HS_VERTICAL);//垂直线填充
	//setfillstyle(BS_HATCHED, HS_FDIAGONAL);//右斜线填充
	//setfillstyle(BS_HATCHED, HS_BDIAGONAL);//左斜线填充
	//setfillstyle(BS_HATCHED, HS_CROSS);//方格线填充
	//setfillstyle(BS_HATCHED, HS_DIAGCROSS);//斜网格线填充
	//solidrectangle(100, 100, 100 + w, 100 + h);
	//fillrectangle(180, 100, 180 + w, 100 + h);

	//3 图像填充
	//IMAGE img;//定义图片存储变量
	//loadimage(&img, L".\\image\\0.png", 20, 20, true);//加载图片，并大小缩放
	//setfillstyle(BS_DIBPATTERN, NULL,&img);
	//solidrectangle(100, 100,100+w , 100+h);
	//fillrectangle(180, 100, 180+w, 100+h );


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}