#include <graphics.h> 
#include <stdio.h>
#include <conio.h>
int main()
{
	initgraph(300, 400, EW_SHOWCONSOLE | EW_DBLCLKS);//创建300*400的窗口
	ExMessage msg;//创建消息变量

	while (1)	// 游戏主循环  可借助break 结束循环
	{
		 
		if (peekmessage(&msg, -1, true))
		{
			switch (msg.message)
			{
			case WM_KEYDOWN:
					switch (msg.vkcode)
					{
					case VK_LEFT:
						printf("按 左\n");
						break;
					case VK_RIGHT:
						printf("按 右\n");
						break;
					case VK_UP:
						printf("按 上\n");
						break;
					case VK_DOWN:
						printf("按 下\n");
						break;
					}
				break;
			case WM_LBUTTONDOWN://左键按下
				printf("左键按下 x:%d y:%d\n", msg.x, msg.y);
				break;
			}

		}

	}
}