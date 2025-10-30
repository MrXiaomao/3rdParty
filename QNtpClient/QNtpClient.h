#ifndef QNTPCLIENT_H
#define QNTPCLIENT_H

#include <QObject>
#include <QDateTime>
class QUdpSocket;

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT NtpClient : public QObject
#else
class QNtpClient : public QObject
#endif

{
    Q_OBJECT
public:
    static QNtpClient *instance();
    explicit QNtpClient(QObject *parent = 0);

private:
    static QScopedPointer<QNtpClient> self;
    QString ntpIP;
    QUdpSocket *udpSocket;

private slots:
    void readData();
    void sendData();

signals:
    //收到时间返回
    void receiveTime(const QDateTime &dateTime);

public slots:
    //设置NTP服务器IP
    void setNtpIP(const QString &ntpIP);

    //获取日期时间
    void getDateTime();

};

#endif // NTPCLIENT_H
