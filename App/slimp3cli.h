#ifndef SLIMP3CLI_H
#define SLIMP3CLI_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QQueue>
#include <QTcpSocket>
#include "squeezedefines.h"

class Slimp3CLI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ServerAddress READ ServerAddress CONSTANT)
    Q_PROPERTY(QString CliPort READ CliPort  CONSTANT)
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged FINAL)

public:
    explicit Slimp3CLI(QObject *parent = nullptr,
                       QString serverAdd = "127.0.0.1",
                       QString cliport = "9090");
    ~Slimp3CLI();

    void Init(void);
    void Connect(void);

    QString ServerAddress() const;
    QString CliPort() const;

    bool isConnected() const;
    void setIsConnected(bool newIsConnected);

signals:
    void Error(QString message);
    void initialized();
    void ResponseReady(QByteArray msg);

    void isConnectedChanged();
    void connectionFailed();

public slots:
    void sendCommand( QByteArray cmd );
    void sendCommand( QByteArray cmd, QByteArray mac );
    void messageWaiting(void);
    void Connected(void);
    void Disconnected(void);
    void ConnectionError(QAbstractSocket::SocketError err);

private:
    SqueezeServer *squeezeServer;   // convenience pointer
    QString m_ServerAddress;
    QString m_CliPort;
    QTcpSocket *m_cliSocket;
    bool m_isConnected;
    int requestCount = 0;
};

#endif // SLIMP3CLI_H
