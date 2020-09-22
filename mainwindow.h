#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include "QFileDialog"
#include "QFile"
#include "QDateTime"
#include "QDebug"
#include "QTime"
#include "QTimer"
#include "QProcess"
#include "QMessageBox"
#include "QSystemTrayIcon"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString file_path;//程序所在路径
    QString file_name;//要守护的程序名

    QString record_path;
    QTimer time_count;
    QProcess p;
    QString tasklist;//当前运行程序

    bool status_before=false;
    bool status_now;
    bool status_start=true;
public:
    void init();
    void write(QString arg);
    void changeEvent(QEvent *event);
private slots:
    //任务栏
    void iconActived(QSystemTrayIcon::ActivationReason reason);

    //
    void listen();
    void on_btn_open_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_btn_start_clicked();

    void on_btn_stop_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
