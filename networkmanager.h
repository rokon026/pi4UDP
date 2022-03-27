#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QWidget>

#include <QUdpSocket>
#include <QHostInfo>
#include <QNetworkAddressEntry>
#include <QTableWidget>
#include <QByteArray>
#include <QStringList>
#include <QProcess>
#include <QTableView>

namespace Ui {
class networkmanager;
}

class networkmanager : public QWidget
{
    Q_OBJECT

public:
    explicit networkmanager(QWidget *parent = 0);
    ~networkmanager();

    QString getLocalIP();
    void sendUDPmessage();
    QString getUserName();

private:
    Ui::networkmanager *ui;

private:
    QTimer *time;
    QString LocalName; //本机名
    QString address; //本机 IP
    QString HostName; //用户名
    QUdpSocket *udpSocket; //udp 套接字
    const static quint16 portm = 55555; //我方端口号
    const static quint16 porto = 45454; //对方端口号
    QHostAddress cliAddr; //对方 IP 地址
private slots:
    void dealUDPreceiveF(); //握手前 UDP 接收处理函数
    void dealUDPreceiveS(); //握手后 UDP 接收处理函数
};

#endif // NETWORKMANAGER_H
