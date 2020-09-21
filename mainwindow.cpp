#include "mainwindow.h"
#include "ui_mainwindow.h"
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

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::init()
{
    //初始化，开启计时器
    time_count.setInterval(1000);//间隔时间

    ui->btn_start->setEnabled(false);

    record_path="C:/Users/Administrator/Desktop/record.txt";
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
    time_count.start();
    connect(&time_count,SIGNAL(timeout()),this,SLOT(listen()));
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    qDebug()<<arg1;
    listen();
}

void MainWindow::on_btn_start_clicked()
{
    if(ui->statusBar->currentMessage()=="Running")
    {
        return;
    }
    qDebug()<<"click start";
    qDebug()<<file_path;
    p.startDetached(file_path);


}

void MainWindow::on_btn_stop_clicked()
{
    QString cmd="taskkill /im "+file_name+" /f";
    p.execute(cmd);

}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.trimmed()!="")
        ui->btn_start->setEnabled(true);
    file_path=arg1;
}
