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
    this->setWindowTitle("Server");
    ui->lineEdit_Port->setText(""); // // Enter the PORT You Want to Connect to (pre-setting)
    ui->pushButton_Send->setEnabled(false); // first enable


    // New TcpServer
    m_server = new QTcpServer();
    connect(m_server,&QTcpServer::newConnection,this,&Widget::server_New_Connect);  // detect new connection -> send to server_New_Connect

}

Widget::~Widget()
{
    m_server->close();
    m_server->deleteLater();
    delete ui;

}

// Import Client Connect
void Widget::server_New_Connect()
{
    m_socket = m_server->nextPendingConnection();

    // Connect
    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &Widget::socket_Recv_Data);
    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &Widget::socket_Disconnect);

    ui->textBrowser->setTextColor(Qt::gray);    // text color
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));   // text font, size
    ui->textBrowser->append(tr("Client Connect: %1\n").arg(QTime::currentTime().toString()));  // client connect: current time

    ui->pushButton_Send->setEnabled(true);  // success the connect and able the button

}

// Receive Socket Data
void Widget:: socket_Recv_Data()
{
    QByteArray data_tmp;
    data_tmp = m_socket->readAll();

    if(!data_tmp.isEmpty())
    {
        QString str = QString(data_tmp);

        ui->textBrowser->setTextColor(Qt::gray);    // text color
        ui->textBrowser->setCurrentFont(QFont("Times New Roman", 10));   // text font
        ui->textBrowser->append("From Client: "+QTime::currentTime().toString());   // from client: current time

        ui->textBrowser->setTextColor(Qt::black);  // text color
        ui->textBrowser->setCurrentFont(QFont("Times New Roman", 14));  // text font
        ui->textBrowser->append(str);

        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(cursor);
    }
}

// Socket Disconnect
void Widget:: socket_Disconnect()
{
    ui->pushButton_Send->setEnabled(false);

    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append(tr("Client Disconnect: %1\n").arg(QTime::currentTime().toString()));  // Client Disconnect

}

// Connect Button - Listen
void Widget::on_pushButton_Listen_clicked()
{
    if (ui->pushButton_Listen->text() ==QString("Connect"))  // Listen
    {
        qint16 port = ui->lineEdit_Port->text().toInt();

        // if connection is failed
        if(!m_server->listen(QHostAddress::Any, port))
        {
            QMessageBox::critical(this,"Error",m_server->errorString(),QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes );
            return;

        }
        ui->pushButton_Listen->setText("Disconnect"); // Cancel the Listen

    }
    else
    {
        if(m_socket->state() == QAbstractSocket::ConnectedState)
        {
            m_socket->disconnectFromHost();
        }
        m_server->close();

        ui->pushButton_Listen->setText("Connect");   // Listen
        ui->pushButton_Send->setEnabled(false);
    }
}

// Send Button
void Widget::on_pushButton_Send_clicked()
{
    // Empty Message detection
    if (ui->textEdit->toPlainText() == QString())
    {
        QMessageBox msgb;
        msgb.setWindowTitle("Server");
        msgb.setText("Can't Send Empty Message!");   // Can't Send Empty Message
        msgb.resize(60,40);
        msgb.exec();
        return;
    }

    m_socket->write(ui->textEdit->toPlainText().toUtf8());

    ui->textBrowser->setTextColor(Qt::gray);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->textBrowser->append("From Server:  "+QTime::currentTime().toString());

    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->setCurrentFont(QFont("Times New Roman",14));
    ui->textBrowser->append(ui->textEdit->toPlainText().toUtf8());

    m_socket->flush();

    ui->textEdit->clear();
    ui->textEdit->setFocus();   // set textEdit cursor activate

}
