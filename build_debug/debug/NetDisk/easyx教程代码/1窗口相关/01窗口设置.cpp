#include <graphics.h>//包含：图像库头文件
#include <stdio.h>
int main01  ()
{
	//initgraph(300, 400);//创建300*400的窗口
	//开关设置
	//initgraph(300, 400, EW_NOCLOSE);//不显示关闭按钮
	//initgraph(300, 400, EW_NOMINIMIZE);//不显示最小化按钮
	// initgraph(300, 400, EW_NOCLOSE| EW_NOMINIMIZE);//组合运用
	initgraph(300, 400, EW_SHOWCONSOLE);//显示控制台
	printf("%d\n",1314);

	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}