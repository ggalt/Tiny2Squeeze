#include "slimp3cli.h"
#include "squeezedefines.h"
#include "squeezeserver.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <chrono>
#include <thread>

#ifdef SLIMCLI_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif


Slimp3CLI::Slimp3CLI(QObject *parent, QString serverAdd, QString cliport)
    : QObject{parent}
{
    m_ServerAddress = serverAdd;
    m_CliPort = cliport;
    squeezeServer = static_cast<SqueezeServer*>(parent); // establish convenience pointer
}

Slimp3CLI::~Slimp3CLI()
{
    disconnect(m_cliSocket, SIGNAL(disconnected()));
    sendCommand("exit");
    m_cliSocket->flush();
    delete m_cliSocket;
}

void Slimp3CLI::Init()
{
    m_cliSocket = new QTcpSocket(this);
    connect( m_cliSocket, SIGNAL(connected()), this, SLOT(Connected()));
    connect( m_cliSocket, SIGNAL( errorOccurred(QAbstractSocket::SocketError)),
            this, SLOT(ConnectionError(QAbstractSocket::SocketError)));
    // squeezeServer->setCliConnected(false);

    setIsConnected(false);
    requestCount = 0;

    Connect();
}

void Slimp3CLI::Connect()
{
    DEBUGF("Connecting CLI at:" << m_ServerAddress << "and" << m_CliPort);
    m_cliSocket->connectToHost(m_ServerAddress, m_CliPort.toInt());
}

QString Slimp3CLI::ServerAddress() const
{
    return m_ServerAddress;
}

QString Slimp3CLI::CliPort() const
{
    return m_CliPort;
}

void Slimp3CLI::sendCommand(QByteArray cmd)
{
    // tempCommand = "subscribe playlist,mixer,pause \n";
    if(!cmd.trimmed().endsWith("\n")) {     // all commands need to end with a newline
        cmd = cmd.trimmed() + "\n";
    }
    if(m_cliSocket->write(cmd) > 0) {
        m_cliSocket->flush();
    }
}

void Slimp3CLI::sendCommand(QByteArray cmd, QByteArray mac)
{
    cmd = mac + " " + cmd;
    sendCommand(cmd);
}

void Slimp3CLI::messageWaiting()
{
    while (m_cliSocket->bytesAvailable()) {
        if( m_cliSocket->canReadLine()) {
            QByteArray msg = m_cliSocket->readLine();
            DEBUGF(msg);
            emit ResponseReady(msg);
        }
    }
}

void Slimp3CLI::Connected()
{
    DEBUGF("CONNECTED!!");
    setIsConnected(true);
    requestCount = 0;
    connect( m_cliSocket, SIGNAL(readyRead()), this, SLOT(messageWaiting()));
    // connect( m_cliSocket, SIGNAL( errorOccurred(QAbstractSocket::SocketError)),
    //         this, SLOT(ConnectionError(QAbstractSocket::SocketError)));

    connect(this, SIGNAL(ResponseReady(QByteArray)),
            squeezeServer, SLOT(receiveCLIResponse(QByteArray)));

    sendCommand("subscribe playlist,mixer,pause \n");

    emit initialized();
}

void Slimp3CLI::Disconnected()
{

}

void Slimp3CLI::ConnectionError(QAbstractSocket::SocketError m_err)
{
   DEBUGF("Connection Error:" << m_err);
    if( !isConnected() && requestCount < 5 ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        requestCount++;
        Connect();
    } else {
        DEBUGF( "CONNECTION ERROR!! " << m_err);
        emit connectionFailed();
    }
}

bool Slimp3CLI::isConnected() const
{
    return m_isConnected;
}

void Slimp3CLI::setIsConnected(bool newIsConnected)
{
    if (m_isConnected == newIsConnected)
        return;
    m_isConnected = newIsConnected;
    emit isConnectedChanged();
}
