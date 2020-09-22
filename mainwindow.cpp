#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMenu"
#include "QObject"
/**************守护程序**********************/
/************mimo431************************/
/************2020-0921-1524****************/
//待改进：
//最小化到任务栏，右键完成各种操作,后台运行
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}


void MainWindow::init()
{
    QSystemTrayIcon *m_trayicon=new QSystemTrayIcon();
    m_trayicon->setToolTip(QString("守护进程"));
    m_trayicon->setIcon(QIcon(":/img/img/App.ico"));
    connect(m_trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActived(QSystemTrayIcon::ActivationReason)));
    m_trayicon->show();

    QMenu *m_traymenu=new QMenu();

    QAction *action_show=new QAction(tr("打开程序"),this);
    QAction *action_quit=new QAction(tr("退出"),this);
    QAction *action_start=new QAction(tr("启动"),this);
    QAction *action_stop=new QAction(tr("停止"),this);
    QAction *action_auto=new QAction(tr("自动"),this);
    //QAction *action_open=new QAction(tr("打开文件"),this);
    action_show->setIcon(QIcon(":/img/img/App.ico"));
    action_auto->setCheckable(true);

    m_traymenu->addAction(action_show);
    m_traymenu->addSeparator();
   // m_traymenu->addAction(action_open);
   // m_traymenu->addSeparator();
    m_traymenu->addAction(action_auto);
    m_traymenu->addAction(action_start);
    m_traymenu->addAction(action_stop);
    m_traymenu->addSeparator();
    m_traymenu->addAction(action_quit);
    m_trayicon->setContextMenu(m_traymenu);//设置上下文菜单
    connect(action_show,&QAction::triggered,this,[=](){
        this->show();
    });
    connect(action_quit,&QAction::triggered,this,[=](){
        this->close();
       // delete  this;
        //delete m_trayicon;
    });
    connect(action_auto,&QAction::triggered,this,[=](){
            if(action_auto->isChecked())
               {
                qDebug()<<action_auto->isChecked();
                on_checkBox_stateChanged(2);
                ui->checkBox->setChecked(true);
            }
            else {
                qDebug()<<!(action_auto->isChecked());
                on_checkBox_stateChanged(0);
                ui->checkBox->setChecked(false);
            }
    });
    //connect(action_quit,SIGNAL(triggered()),this,SLOT(on_checkBox_stateChanged(2)));
    connect(action_start,&QAction::triggered,this,&MainWindow::on_btn_start_clicked);
    connect(action_stop,&QAction::triggered,this,&MainWindow::on_btn_stop_clicked);
    connect(action_auto,&QAction::triggered,this,&MainWindow::on_checkBox_stateChanged);
//    m_traymenu->addSeparator();//设置一个分割条作为演示
//    //增加一个二级菜单，以此类推可以实现多级菜单
//    QMenu *m_traymenu1 = m_traymenu->addMenu("1_0");
//    //一个更快捷添加菜单项的方法
//    m_traymenu1->addAction("1_1",this,SLOT(close));
    //初始化，开启计时器
    time_count.setInterval(1000);//间隔时间

    ui->btn_start->setEnabled(false);

    record_path="C:/Users/Administrator/Desktop/record.txt";
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::listen()
{
    p.start("tasklist",QStringList()<<"-fi"<<"imagename eq "+file_name);
    p.waitForFinished();
    tasklist=QString::fromLocal8Bit(p.readAllStandardOutput());
    status_now=(tasklist.contains(file_name));
    qDebug()<<"listening..."<<status_before<<"   "<<status_now<<"  "<<ui->checkBox->checkState();

//    //初始化阶段
//    if(status_start)
//    {
//        if(tasklist.contains(file_name))
//        {
//            ui->statusBar->showMessage("Running");
//        }
//        else {
//            ui->statusBar->showMessage("Stop");
//        }
//        status_start=false;
//        write("守护程序启动晚于软件启动时间，开启时间未知");
//        tasklist.clear();
//        status_before=status_now;
//        return;
//    }
    //自动状态较为特殊，前后皆为true,
    if(Qt::Checked==ui->checkBox->checkState())
        if(status_now==false)
        {
            on_btn_start_clicked();
        }
    if(status_now!=status_before)
    {
       if(status_now==true){
        //  write("first");
          ui->statusBar->showMessage("Running");
          write("Start:");
       }
       else
       {
            ui->statusBar->showMessage("Stop");
            write("Stop:");
            if(Qt::Checked==ui->checkBox->checkState())
                on_btn_start_clicked();
       }
    }
    tasklist.clear();
    status_before=status_now;

}

void MainWindow::write(QString arg)
{
    QDateTime curTime=QDateTime::currentDateTime();
    QString time_start=curTime.toString("yyyy-MM-dd hh:mm:ss");
    QFile file_record(record_path);
    if(!file_record.open(QIODevice::Append |QIODevice::Text))
    {
        qDebug()<<"someting wrong";
    }
    QTextStream in(&file_record);
    in.setAutoDetectUnicode(true);
    in<<file_name+" "+arg+": "+time_start<<"\n";
    file_record.close();
}
void MainWindow::on_btn_open_clicked()
{
    file_path=QFileDialog::getOpenFileName(this,"找文件","C:/Users/Administrator/Desktop","*.exe");
    ui->lineEdit->setText(file_path);
    file_path=ui->lineEdit->text();
    file_name=file_path.split("/").last();
    qDebug()<<file_name;
    if(file_path!="")
    {
        time_count.start();
        connect(&time_count,SIGNAL(timeout()),this,SLOT(listen()));
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    qDebug()<<"check:"<<arg1;
    if(arg1==2)
        listen();
}

void MainWindow::on_btn_start_clicked()
{
    if(ui->statusBar->currentMessage()=="Running")
    {
        return;
    }
    p.start("tasklist",QStringList()<<"-fi"<<"imagename eq "+file_name);
    p.waitForFinished();
    tasklist=QString::fromLocal8Bit(p.readAllStandardOutput());
    if(tasklist.contains(file_name))
    {
        return;
    }
    qDebug()<<"click start";
    qDebug()<<file_path;
    qDebug()<<p.startDetached(file_path,QStringList());
}

void MainWindow::on_btn_stop_clicked()
{
    QString cmd="taskkill /im "+file_name+" /f";
    qDebug()<<p.execute(cmd);
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.trimmed()!="")
        ui->btn_start->setEnabled(true);
    file_path=arg1;
}

void MainWindow::iconActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    //双击托盘显示窗口
    case QSystemTrayIcon::DoubleClick:
    {
        this->show();
        break;
    }
    default:
        break;
    }
}
void MainWindow::changeEvent(QEvent *event)
{
    if(event->type()!=QEvent::WindowStateChange)
        return;
    if(this->windowState()==Qt::WindowMinimized)
        this->hide();
}
