
#include<graphics.h>
#pragma comment(lib,"Winmm.lib")

int main()
{
	//打开音乐                                   别名  
	mciSendString(L"open .\\music\\背景乐_01.mp3 alias bk", 0, 0, 0);
	//播放音乐
	mciSendString(L"play bk ", 0, 0, 0); 
	Sleep(20*1000);//播放20秒
	//关闭音乐
	mciSendString(L"close bk", 0, 0, 0);
	return 0;
}