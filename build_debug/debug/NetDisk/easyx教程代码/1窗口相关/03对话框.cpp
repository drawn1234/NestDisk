#include <graphics.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
int main03  ()
{
	initgraph(300, 400, EW_SHOWCONSOLE);//显示控制台
	setlocale(LC_ALL, "chs");
	/*bool InputBox(
	LPTSTR	接收用户输入的字符数组,
	int		输入内容缓冲区大小,
	LPCTSTR	提示信息,
	LPCTSTR	提示标题,
	LPCTSTR	输入的默认值,
	int		0 默认宽度,
	int		0 默认高度,
	bool	true 隐藏取消按钮   false 确认 取消 
);*/
	TCHAR info[50];//创建一个双字节字符数组，用以保留用户的输入
	bool answer;
	 answer = 	InputBox(info,50, L"输入你感兴趣的岗位", L"入职调查", L"C++后端",0,0,true);
	printf("%S\n", info);//%S 以双字节打印字符串 %s以单字节打印字符串

	answer = InputBox(info, 50, L"期望薪资（K）", L"目标", L"20", 0, 0, false);
	int sal = _wtoi(info);//将用户输入的字符串转int型整数
	printf("%d\n",sal );
	printf("选择了：“%s” 按钮\n", answer ?  "确定" : "取消"); 
 


	system("pause");//暂停: 避免窗口自动关闭
	return 0;
}