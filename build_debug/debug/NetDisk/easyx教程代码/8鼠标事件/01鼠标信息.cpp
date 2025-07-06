#include <graphics.h> 
#include <stdio.h>
int main ()
{
					//显示控制台 并且 支持双击事件
	initgraph(300, 400, EW_SHOWCONSOLE| EW_DBLCLKS); 
	ExMessage mouse;//创建消息变量

	while (1)	// 游戏主循环  可借助break 结束循环
	{
		//把鼠标的消息 存入变量
		if (peekmessage(&mouse, EM_MOUSE, true))
		{
			switch (mouse.message)
			{
			case WM_MOUSEMOVE://鼠标移动
				printf("鼠标移动 x:%d y:%d\n", mouse.x, mouse.y);
				break;
			case WM_LBUTTONDOWN://左键按下
				printf("左键按下 x:%d y:%d\n",mouse.x ,mouse.y );
				break;
			case WM_LBUTTONUP://左键抬起
				printf("左键抬起 x:%d y:%d\n", mouse.x, mouse.y);
				break;
			case WM_LBUTTONDBLCLK://左键双击
				printf("左键双击 x:%d y:%d\n", mouse.x, mouse.y);
				break;
			case WM_RBUTTONDOWN://右键按下
				printf("右键按下 x:%d y:%d\n", mouse.x, mouse.y);
				break;
			case WM_RBUTTONUP://右键抬起
				printf("右键抬起 x:%d y:%d\n", mouse.x, mouse.y);
				break;
			case WM_RBUTTONDBLCLK://右键双击
				printf("右键双击 x:%d y:%d\n", mouse.x, mouse.y);
				break;

			 
			}

		}
	}

	return 0;
}