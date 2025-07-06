#include <graphics.h>//包含：图像库头文件
void drawImageAlpha(int x , int y , IMAGE* img, double f);
int main06 ()
{
	initgraph(300, 400);//创建300*400的窗口
	//按图片默认大小加载
	IMAGE img1;
	loadimage(&img1, L".\\image\\乌龟_15.png");//从硬盘任意位置加载图像，并默认图像大小 
	

	//正常不透明绘图
	putimage( 0, 200, &img1);

	//半透明绘图
	drawImageAlpha(0, 0, &img1, 0.5);

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}

/*调整图片透明度*/
void drawImageAlpha(int x0, int y0, IMAGE* img, double f)
{
	//获得图片尺寸
	int qwidth, qheight;
	qwidth = img->getwidth();
	qheight = img->getheight();


	//P图为背景图,RS为目标图片
	static IMAGE RS(qwidth, qheight);
	static IMAGE P(qwidth, qheight);


	//背景图的绘制
	getimage(&P, x0, y0, qwidth, qheight);


	//获取指针，作为透明度计算
	DWORD* M = GetImageBuffer(&P);
	DWORD* N = GetImageBuffer(img);
	DWORD* R = GetImageBuffer(&RS);


	// 开启批量绘图模式，解决闪烁问题
	BeginBatchDraw();


	//计算与赋值
	int i, j;
	for (i = 0; i < qheight; i++) {
		for (j = 0; j < qwidth; j++) {
			int r, g, b;
			int ij;
			//计算
			ij = i * qwidth + j;
			r = (int)((GetRValue(N[ij])) * (1 - f) + GetRValue(M[ij]) * f);
			g = (int)((GetGValue(N[ij])) * (1 - f) + GetGValue(M[ij]) * f);
			b = (int)((GetBValue(N[ij])) * (1 - f) + GetBValue(M[ij]) * f);
			R[ij] = RGB(r, g, b);
		}
	}
	//贴出图片并释放内存
	putimage(x0, y0, &RS);
	FlushBatchDraw();// 绘制
}