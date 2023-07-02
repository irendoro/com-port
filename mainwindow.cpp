#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        {
        ui->setPortName->addItem(serialPortInfo.portName());
        }

    foreach (const qint32 baudRate, QSerialPortInfo::standardBaudRates()) {
        ui->setbaudrate->addItem(QString::number(baudRate));
        }
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    ui->setPortName->setCurrentIndex(0);
    ui->setbaudrate->setCurrentIndex(12);
    ui->setdatabits->setCurrentIndex(3);
    ui->setparity->setCurrentIndex(0);
    ui->setstopbits->setCurrentIndex(0);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);

}


MainWindow::~MainWindow()
{
    serial->close();
    delete ui;
}

//Подключение к COM-port, при успешном подключении - деактивация кнопок выбора настроек
void MainWindow::on_Connect_clicked()
{
    if (!serial->open(QIODevice::ReadWrite)){
        return;
    }
    else {
        ui->setPortName->setEnabled(false);
        ui->setbaudrate->setEnabled(false);
        ui->setdatabits->setEnabled(false);
        ui->setparity->setEnabled(false);
        ui->setstopbits->setEnabled(false);
        ui->Connect->setEnabled(false);
        ui->Disconnect->setEnabled(true);
        statusBar()->showMessage("Success! COM-PORT is opened.");
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::receiveMessage);
    }

}

// Выход из программы
void MainWindow::on_Exit_clicked()
{
    close();
}

//Выбор COM-port в выпадающем списке
void MainWindow::on_setPortName_activated(const QString &arg1)
{
    serial->setPortName(arg1);
    if (serial->isOpen())
        serial->close();
}

//Выбор Baudrate в выпадающем списке
void MainWindow::on_setbaudrate_activated(const QString &arg1)
{
    int n = arg1.toInt();
    serial->setBaudRate(n);
}

//Выбор stopbits в выпадающем списке
void MainWindow::on_setstopbits_activated(const QString &arg1)
{
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    if (arg1 == "1")
        stopBits = QSerialPort::OneStop;
    else if (arg1 == "1.5")
        stopBits = QSerialPort::OneAndHalfStop;
    else if (arg1 == "2")
        stopBits = QSerialPort::TwoStop;
    serial->setStopBits(stopBits);
}

//Выбор parity в выпадающем списке
void MainWindow::on_setparity_activated(const QString &arg1)
{
    QSerialPort::Parity parity = QSerialPort::NoParity;
    if (arg1 == "None")
        parity = QSerialPort::NoParity;
    else if (arg1 == "Odd")
        parity = QSerialPort::OddParity;
    else if (arg1 == "Even")
        parity = QSerialPort::EvenParity;
    else if (arg1 == "Space")
        parity = QSerialPort::SpaceParity;
    else if (arg1 == "Mark")
        parity = QSerialPort::MarkParity;
    serial->setParity(parity);
}

//Выбор databits в выпадающем списке
void MainWindow::on_setdatabits_activated(const QString &arg1)
{
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    if (arg1 == "5")
        dataBits = QSerialPort::Data5;
    else if (arg1 == "6")
        dataBits = QSerialPort::Data6;
    else if (arg1 == "7")
        dataBits = QSerialPort::Data7;
    else if (arg1 == "8")
        dataBits = QSerialPort::Data8;
    serial->setDataBits(dataBits);
}

//Кнопка настройки, вывод окна с выбранными настройками COM-port
void MainWindow::on_settings_clicked()
{
    QString portSettings = QString("Port Name: %1\n""Baud Rate: %2\nData Bits: %3\nParity: %4\n"
                                           "Stop Bits: %5\n")
                                           .arg(serial->portName())
                                           .arg(serial->baudRate())
                                           .arg(serial->dataBits())
                                           .arg(serial->parity())
                                           .arg(serial->stopBits());
    QMessageBox::information(this, "Port Settings", portSettings);
}

//Очищение поля ввода сообщения
void MainWindow::on_clear_clicked()
{
    ui->textEdit->clear();
}

//Отправка сообщения на COM-port
void MainWindow::on_send_clicked()
{
    if (serial->isOpen()) {
        QString message = ui->textEdit->toPlainText();
        QByteArray byteArray;
        qDebug() << "Send Data";
        if (ui->ascii->isChecked()) {
            byteArray = message.toUtf8();
        } else {
            byteArray = QByteArray::fromHex(message.toUtf8());
        }
        serial->write(byteArray);
        if (serial->waitForBytesWritten())
            statusBar()->showMessage("Success!");
        else
            statusBar()->showMessage("Unsuccess!");
    }
    else {
        statusBar()->showMessage("Unsuccess! COM-PORT is closed."); //должна ли здесь выводиться ошибка?
    }
}

//Получение сообщения с COM-port
void MainWindow::receiveMessage()
{
    if (serial->isOpen()) {
        QByteArray responceData = serial->readAll();
        if (!responceData.isEmpty())
        {
            QString Responce;
            if (ui->ascii_1->isChecked()) {
                Responce=QString(responceData);
            } else {
                Responce=QString(responceData.toHex());
            }
            ui->textEdit_2->setText(Responce);
        }
        else
        {   qDebug() << "Empty";
            return;
        }

    }
}

//Вывод сообщения об ошибке
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
        return;
    statusBar()->showMessage("Serial Port Error: " + serial->errorString());
}

//Отключение от COM-port, при успешном отключении - активация кнопок выбора настроек
void MainWindow::on_Disconnect_clicked()
{
    if (serial->isOpen())
        serial->close();
    ui->Connect->setEnabled(true);
    ui->Disconnect->setEnabled(false);
    ui->setPortName->setEnabled(true);
    ui->setbaudrate->setEnabled(true);
    ui->setdatabits->setEnabled(true);
    ui->setparity->setEnabled(true);
    ui->setstopbits->setEnabled(true);
}
