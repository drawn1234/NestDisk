#include"ckernel.h"
#include <QApplication>
#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //获取对象-创建-构造-窗口创建在构造中实现
    //窗口的对象使用完需要回收-在cKernel函数中创建-避免在主函数中创建-主函数不退出就不会回收
    //在CKernel-构造创建-析构手动回收-窗口生命周期=CKernel
    //2: 使用方法回收，关闭窗口，回收窗口对象-使用信号与槽
    CKernel::GetInstance();
    return a.exec();
}
