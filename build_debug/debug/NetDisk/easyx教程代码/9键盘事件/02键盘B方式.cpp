#include <graphics.h> 
#include <stdio.h>
#include <conio.h>
int main02()
{
	initgraph(300, 400, EW_SHOWCONSOLE | EW_DBLCLKS);//创建300*400的窗口
	ExMessage key;//创建消息变量

	while (1)	// 游戏主循环  可借助break 结束循环
	{
		 
		if (peekmessage(&key, EM_KEY, true))
		{
			if(key.message== WM_KEYDOWN)
			{
				switch (key.vkcode)
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

				switch (key.vkcode)
				{
				case 'A':
				case 'a':
					printf("按 A\n");
					break;
				case 'B':
				case 'b':
					printf("按 B\n");
					break;
				case 'c':
				case 'C':
					printf("按 C\n");
					break;
				case 'D':
				case 'd':
					printf("按 d\n");
					break;
				}
			}
		}

	}
}