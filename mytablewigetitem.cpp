#include "mytablewigetitem.h"

MytablewigetItem::MytablewigetItem()
{}

void MytablewigetItem::slot_setFile(FileInfo &file)
{
    //设置传输列表中的文件信息
    //作为第一列出现 文件名
    m_file=file;
    this->setText(m_file.name);
    //设置图标
    if(file.type=="file"){
        this->setIcon(QIcon(":/images/file.png"));
    }else if(file.type=="folder"){
        this->setIcon(QIcon(":/images/folder.png"));
    }
    //勾选
    this->setCheckState(Qt::Unchecked);//设置为未勾选

}
