#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets>
#include <QMessageBox>
#include <QTime>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("Client");
    m_socket=new QTcpSocket();

    QObject::connect(m_socket,&QTcpSocket::readyRead,this,&Widget::sockt_recv_data);
    QObject::connect(m_socket,&QTcpSocket::disconnected,this,&Widget::socket_disconnect);

    ui->pushButton_Send->setShortcut(QKeySequence(tr("ctrl+return")));

    ui->lineEdit_IP->setText("");   // Enter the IP You Want to Connect to (pre-setting)
    ui->lineEdit_Port->setText(""); // Enter the PORT You Want to Connect to (pre-setting)
    ui->pushButton_Send->setEnabled(false);
}

Widget::~Widget()
{
    delete m_socket;
    delete ui;
}

// Connect Button
void Widget::on_pushButton_Connect_clicked()
{
    QString IP;
    qint16 port;

    if (ui->pushButton_Connect->text() == QString("Connect"))    // connect
    {
        IP=ui->lineEdit_IP->text();
        port=ui->lineEdit_Port->text().toInt();

        m_socket->abort();
        m_socket->connectToHost(IP,port);

            if (!m_socket->waitForConnected())
                {

                    QMessageBox msgBox;
                     msgBox.setWindowTitle("Client");
                     msgBox.setText("Runtime Out");   // connect runtime out
                     msgBox.resize(40,30);
                     msgBox.exec();
                    return;
                }

        QMessageBox msgBox;
        msgBox.setWindowTitle("Client");
        msgBox.setText("Successful Connection");  // Connect Success
        msgBox.resize(40,30);
        msgBox.exec();

        ui->pushButton_Send->setEnabled(true);
        ui->pushButton_Connect->setText("Disconnect");
       }
    else
    {
        m_socket->disconnectFromHost();
        ui->pushButton_Connect->setText("Connect");
        ui->pushButton_Send->setEnabled(false);
    }
}

void Widget::on_pushButton_Send_clicked()  // Send Button
{
    if (ui->textEdit->toPlainText()== QString())    // Detect Empty Message
    {
        QMessageBox msgb;
        msgb.setText("Can't Send Empty Message");   // Cant send Empty Message
        msgb.resize(60, 40);
        msgb.exec();
        return;
    }
    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman", 10));
    ui->textBrowser->append("From Client:  " + QTime::currentTime().toString());

    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman", 14));
    ui->textBrowser->append(ui->textEdit->toPlainText().toUtf8());

    m_socket->write(ui->textEdit->toPlainText().toUtf8());
    m_socket->flush();

    ui->textEdit->clear();
    ui->textEdit->setFocus();   // set textEdit cursor activate

}

void Widget:: sockt_recv_data()
{
    QByteArray data_tmp;
    data_tmp = m_socket->readAll();
    if (!data_tmp.isEmpty())
    {
        QString str=QString(data_tmp);

        ui->textBrowser->setTextColor(Qt::gray);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman", 10));
        ui->textBrowser->append("From Server:  " + QTime::currentTime().toString());

        ui->textBrowser->setTextColor(Qt::black);
        ui->textBrowser->setCurrentFont(QFont("Times New Roman", 14));
        ui->textBrowser->append(str);

    }
}

void Widget:: socket_disconnect()
{
    ui->pushButton_Send->setEnabled(false);
    ui->pushButton_Connect->setText("Connect");  // Connect
    QMessageBox msgBox;
    msgBox.setWindowTitle("CAUTION");
    msgBox.setText("Closed");   // Disconnect
    msgBox.resize(40,30);
    msgBox.exec();
}
