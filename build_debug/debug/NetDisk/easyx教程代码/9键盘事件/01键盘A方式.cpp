#include <graphics.h>//包含：图像库头文件
#include <stdio.h>
#include <conio.h>
int main01()
{
	initgraph(300, 400,EW_SHOWCONSOLE);//创建300*400的窗口

	while (1)
	{
		if (_kbhit())
		{
			char key = _getch();
			switch (key)
			{
			case 'A':
			case 'a':
				printf("左\n");
				break;
			case 'D':
			case 'd':
				printf("右\n");
				break;
			case 'W':
			case 'w':
				printf("上\n");
				break;
			case 'S':
			case 's':
				printf("下\n");
				break;
			case 72: //上

				break;
			case 80://下

				break;
			case 75://左

				break;
			case 77://右

				break;
			}
		}
	}


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}