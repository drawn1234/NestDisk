#include "playerdialog.h"
#include "ui_playerdialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QStyle>
#include <QMouseEvent>

#define _DEF_PATH "D:\BaiduNetdiskDownload\20241128\kk 2024-11-28 18-15-20.mp4"
//#define _DEF_PATH "H:/乱七八糟/曾经我也想过一了百了.mp3"
#define PATH "rtmp://192.168.81.174:1935/vod/101.mp4"
#define PATH_ON "http://192.168.91.239:80/hls/101.m3u8 "
PlayerDialog::PlayerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlayerDialog)
{    qDebug()<<__func__;

    ui->setupUi(this);
    m_player=new VideoPlayer;

    connect(m_player,SIGNAL(SIG_getOneImage(QImage)),this,SLOT(slot_setImage(QImage)));
    slot_PlayerStateChanged(PlayerState::Stop);
    //connect(&m_timer,SIGNAL(timeout()),this,SLOT());
    connect(m_player,SIGNAL(SIG_PlayerStateChanged(int)),this,SLOT(slot_PlayerStateChanged(int)));

    //void SIG_TotalTime(qint64 uSec);
    connect( m_player, SIGNAL( SIG_TotalTime(qint64)) ,this ,SLOT( slot_getTotalTime(qint64)) );

    connect(&m_timer,SIGNAL(timeout()),this,SLOT(slot_TimerTimeOut()));
    m_timer.setInterval(500);  //超时时间500ms

    //安装事件过滤器，让该对象成为被观察对象  this去执行函数
    ui->slider_progress->installEventFilter(this);
}

PlayerDialog::~PlayerDialog()
{    qDebug()<<__func__;

    delete ui;
    delete m_player;
}

void PlayerDialog::close()
{
    qDebug()<<__func__;
    m_player->stop(true);
}

//Qt 线程
//QThread 定义子类 start() -> run()
void PlayerDialog::on_pb_start_clicked()  //打开
{
    qDebug()<<__func__;
    //开始播放  -》 一段时间内获取图片
    //m_player->start();
    //首先  先关闭
    if(m_player->playerState()!=PlayerState::Stop)
    {
        m_player->stop(true);
    }
    //打开浏览选择文件
    QString path=QFileDialog::getOpenFileName(
        this,
        "打开文件",
        "./",
        "视频文件 (*.mp4 *.avi *.mkv *.mov *.flv *.wmv *.rmvb *.mpeg *.mpg);;所有文件 (*)"
        );

    //判断
    if(path.isEmpty()) return;
    //设置 m_play fileName
    m_player->setFileName(path);
    //m_player->setFileName(PATH);
    //m_player->setFileName(PATH_ON);
    //m_player->start();
    slot_PlayerStateChanged(PlayerState::Playing);
}

void PlayerDialog::slot_setImage(QImage img)
{
    qDebug()<<__func__;
    //pixmap和image
    //缩放  等比例
    QPixmap pixmap;
    if(!img.isNull())
    {
        pixmap=QPixmap::fromImage(img.scaled(ui->lb_show->size(),Qt::KeepAspectRatio));
    }
    else
    {
        pixmap=QPixmap::fromImage(img);
    }
    ui->lb_show->setPixmap(pixmap);
}


void PlayerDialog::on_pb_resume_clicked()
{
    qDebug()<<__func__;

    if(m_player->playerState()!=PlayerState::Pause) return;
    m_player->play();
    //切换
    ui->pb_resume->hide();
    ui->pb_pause->show();
}


void PlayerDialog::on_pb_pause_clicked()
{
    qDebug()<<__func__;

    if(m_player->playerState()!=PlayerState::Playing) return;
    m_player->pause();
    //切换
    ui->pb_pause->hide();
    ui->pb_resume->show();
}


void PlayerDialog::on_pb_stop_clicked()
{
    qDebug()<<__func__;
    m_player->stop(true);
}

void PlayerDialog::slot_PlayerStateChanged(int state)
{    qDebug()<<__func__;

    switch( state )
    {
    case PlayerState::Stop:
        qDebug()<< "VideoPlayer::Stop";  
        m_timer.stop();

        ui->slider_progress->setValue(0);
        ui->lb_totalTime->setText("00:00:00");
        ui->lb_curTime->setText("00:00:00");
        ui->pb_pause->hide();
        ui->pb_resume->show();
    {//临时创建的黑色图片
        QImage img;
        img.fill( Qt::black);
        slot_setImage(img);
    }
        this->update();
        isStop = true;
        break;
    case PlayerState::Playing:
        qDebug()<< "VideoPlayer::Playing";
        ui->pb_resume->hide();
        ui->pb_pause->show();
        m_timer.start();

        this->update();
        isStop = false;
        break;
    }
}

void PlayerDialog::slot_getTotalTime(qint64 uSec)  //uSec是微秒级别
{    qDebug()<<__func__;

    qint64 Sec = uSec/1000000;
    ui->slider_progress->setRange(0,Sec);//精确到秒
    QString hStr = QString("00%1").arg(Sec/3600);
    QString mStr = QString("00%1").arg(Sec/60);
    QString sStr = QString("00%1").arg(Sec%60);
    QString str = QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
    ui->lb_totalTime->setText(str);
}
//获取当前视频时间定时器
void PlayerDialog::slot_TimerTimeOut()
{    qDebug()<<__func__;

    if (QObject::sender() == &m_timer)  //拿到发送信号的对象的地址
    {
        qint64 Sec = m_player->getCurrentTime()/1000000;
        ui->slider_progress->setValue(Sec);
        QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60%60);
        QString sStr = QString("00%1").arg(Sec%60);
        QString str =QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
        ui->lb_curTime->setText(str);
        if(ui->slider_progress->value() == ui->slider_progress->maximum()
                && m_player->playerState() == PlayerState::Stop)
        {
            slot_PlayerStateChanged( PlayerState::Stop );
        }else if(ui->slider_progress->value() + 1 ==
                 ui->slider_progress->maximum()
                 && m_player->playerState() == PlayerState::Stop)
        {
            slot_PlayerStateChanged( PlayerState::Stop );
        }
    }
}

bool PlayerDialog::eventFilter(QObject *obj, QEvent *event)
{    qDebug()<<__func__;

    if(obj==ui->slider_progress)
    {
        if(event->type()==QEvent::MouseButtonPress)
        {
            QMouseEvent* mouseEvent=static_cast<QMouseEvent*>(event);
            int min=ui->slider_progress->minimum();
            int max=ui->slider_progress->maximum();
            int value=QStyle::sliderValueFromPosition(min,max,mouseEvent->pos().x(),ui->slider_progress->width());
            m_timer.stop();  //如果不stop的话点击跳转的位置后进度条会到达位置再回去一下（因为定时器到了）再回来，所以可以先暂停，设置之后再恢复
            ui->slider_progress->setValue(value);
            m_player->seek((qint64)value*1000000);
            m_timer.start();
            return true;
        }
        else
        {
            return false;
        }
    }
    //可以完善空格暂停和恢复  左右 快进快退 上下音量调整等
    return QDialog::eventFilter(obj,event);
}
