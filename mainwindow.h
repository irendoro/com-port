#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void receiveMessage();
    void handleError(QSerialPort::SerialPortError error);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_Connect_clicked();

    void on_Exit_clicked();

    void on_pushButton_clicked();

    void on_Choise_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_setPortName_activated(const QString &arg1);

    void on_setbaudrate_activated(const QString &arg1);

    void on_comboBox_3_activated(const QString &arg1);

    void on_setstopbits_activated(const QString &arg1);

    void on_setparity_activated(const QString &arg1);

    void on_setdatabits_activated(const QString &arg1);

    void on_settings_clicked();

    void on_clear_clicked();


    void on_send_clicked();

    void on_ascii_clicked();



    void on_Disconnect_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
};

#endif // MAINWINDOW_H
