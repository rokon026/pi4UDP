#include "networkmanager.h"
#include "ui_networkmanager.h"

networkmanager::networkmanager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::networkmanager)
{
    ui->setupUi(this);

    QString HostName = getUserName();
    QTableWidgetItem *myho = new QTableWidgetItem(HostName);
    QString address = getLocalIP();
    QTableWidgetItem *myadd = new QTableWidgetItem(address);
    QString LocalName = QHostInfo::localHostName();
    QTableWidgetItem *mylo = new QTableWidgetItem(LocalName);
    ui->MasterTable->insertRow(0);
    ui->MasterTable->setItem(0,0,myho);
    ui->MasterTable->setItem(0,1,mylo);
    ui->MasterTable->setItem(0,2,myadd);
    udpSocket = new QUdpSocket(this); //分配空间，指定父对象
    udpSocket->bind(QHostAddress::AnyIPv4,portm); //UDP 绑定我方端口号
    udpSocket->joinMulticastGroup( QHostAddress("255.255.255.255") ); //加入一个组播
    connect(udpSocket,&QUdpSocket::readyRead, //当 UDP 接收到信息时会触发 readyRead
            this,&networkmanager::dealUDPreceiveF); //UDP 接收槽函数连接
}

networkmanager::~networkmanager()
{
    delete ui;
}
/*************************************************
* 函数名：QString networkmanager::getLocalIP()
* 函数功能：获取本机 IP 地址
* 输入参数：void
* 输出参数：void
* 返回值：Qstring 类型的 IP 地址
*************************************************/
QString networkmanager::getLocalIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)//得到 IPv4 地址进
        {
            QString useless = address.toString();
            if(useless == "127.0.0.1")//得到的是回环地址舍弃该值
                continue;
            return address.toString();
        }
    }
    return "0.0.0.0";
}
/*************************************************
* 函数名：void networkmanager::dealUDPreceive()
* 函数功能：UDP 接收到信息后进行处理
* 输入参数：void
* 输出参数：获取对方发送信息、ip
* 返回值：void
*************************************************/
void networkmanager::dealUDPreceiveF()
{
    char buf[1024] = {0}; //接收数组
    qint64 len = udpSocket->readDatagram(buf, sizeof(buf), &cliAddr);
    if(len > 0)
    {
        QString str = QString("%1").arg(buf);
        if(str == "hello") //第一次握手成功 并回应对方“你好”
        {
            QByteArray reply = "你好";
            udpSocket->writeDatagram(reply.data(),reply.size(),
                                     QHostAddress(cliAddr), porto);
            disconnect(udpSocket,&QUdpSocket::readyRead,
                       this,&networkmanager::dealUDPreceiveF);
            //第一次握手后断开槽函数
            sendUDPmessage();
            connect(udpSocket,&QUdpSocket::readyRead,
                    this,&networkmanager::dealUDPreceiveS);
        }
    }
}
void networkmanager::dealUDPreceiveS()
{
    QByteArray datagram; //定义接收数据数组
    datagram.resize(udpSocket->pendingDatagramSize()); //定义接收数组大小
    udpSocket->readDatagram(datagram.data(), datagram.size()); //将接收数据放到数组中
    QString Add = QString(datagram).section("##", 0, 0);
    QString local = QString(datagram).section("##", 1, 1);
    QString Host = QString(datagram).section("##", 2, 2);
    QTableWidgetItem * youadd = new QTableWidgetItem(Add);
    QTableWidgetItem * youlo = new QTableWidgetItem(local);
    QTableWidgetItem * youho = new QTableWidgetItem(Host);
    ui->SlaverTable->insertRow(0);
    ui->SlaverTable->setItem(0,0,youlo);
    ui->SlaverTable->setItem(0,1,youho);
    ui->SlaverTable->setItem(0,2,youadd);
}
/*************************************************
* 函数名：void networkmanager::sendUDPmessage()
* 函数功能：UDP 发送数据
* 输入参数：void
* 输出参数：void
* 返回值：void
*************************************************/
void networkmanager::sendUDPmessage()
{
    QString HostName = getUserName();
    QString address = getLocalIP();
    QString LocalName = QHostInfo::localHostName();
    QString ii = QString("%1##%2##%3").arg(address).arg(HostName).arg(LocalName);
    udpSocket->writeDatagram(ii.toUtf8(),ii.size(),
                             QHostAddress(cliAddr), porto);//单播
}
/*************************************************
* 函数名：QString networkmanager::getUserName()
* 函数功能：获取主机用户名
* 输入参数：void
* 输出参数：void
* 返回值：QString 类型的用户名
*************************************************/
QString networkmanager::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1)
        {
            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return "unknown";
}
