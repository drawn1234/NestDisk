#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h> 
int main02()
{
	initgraph(500, 400);
	IMAGE imgs[12];//图片数组 保留12张动画帧的图像。
	/*加载动画的每一帧到数组*/
	//char
	TCHAR path[100];//双字节字符数组临时保留图片的硬盘存储路径
	for (int i = 0; i < 12; i++)
	{
		//_stprintf 实现路径的格式化模板，并把变化的位置设置变量值
		_stprintf(path, L".\\image\\蝴蝶鱼_%d.png", i);//动态拼装图片路径
		loadimage(&imgs[i], path);//根据拼装路径加载图片到图片数组。
	}

	int imgnum = 0;//记录当前动画的第几帧
	/*循环绘制动画的每一帧图像*/
	while (1)
	{
		putimage(20, 30, &imgs[imgnum]);//绘制动画当前帧图像
		Sleep(100);//等待100毫秒
		//切换动画帧 
		if (imgnum == 11)//如果最后一张，就切回第0张
		{
			imgnum = 0;
		}
		else
		{
			imgnum++;
		}
		cleardevice();//清屏
	}


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}