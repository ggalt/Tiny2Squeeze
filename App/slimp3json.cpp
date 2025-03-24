#include "slimp3json.h"
#include "squeezeserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <chrono>
#include <thread>

#ifdef SLIMJSON_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif


Slimp3JSON::Slimp3JSON(QObject *parent, QString serverAddress, QString jsonPort)
    : QObject{parent}
{
    m_ServerAddress = serverAddress;
    m_JSONPort = jsonPort;
    squeezeServer = static_cast<SqueezeServer*>(parent); // establish convenience pointer
    requestCount = 0;
}

void Slimp3JSON::Init()
{
    mgr = new QNetworkAccessManager(this);
    url = QString("http://" + m_ServerAddress+":"+m_JSONPort+"/jsonrpc.js");
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(receiveJSONReply(QNetworkReply*)));
}

QString Slimp3JSON::ServerAddress() const
{
    return m_ServerAddress;
}

void Slimp3JSON::setServerAddress(const QString &newServerAddress)
{
    if (m_ServerAddress == newServerAddress)
        return;
    m_ServerAddress = newServerAddress;
    emit ServerAddressChanged();
}

QString Slimp3JSON::JSONPort() const
{
    return m_JSONPort;
}

void Slimp3JSON::setJSONPort(QString newJSONPort)
{
    if (m_JSONPort == newJSONPort)
        return;
    m_JSONPort = newJSONPort;
    emit JSONPortChanged();
}

QByteArray Slimp3JSON::formatJSONCommand(QString mac, QStringList cmds)
{
    QJsonObject jsonObj;
    jsonObj["id"] = 1;
    jsonObj["method"] = "slim.request";
    QJsonArray commandList;
    foreach( QString str, cmds) {
        commandList.append(str);
    }
    QJsonArray params = {mac,commandList};
    jsonObj["params"] = params;
    QJsonDocument jsonDoc(jsonObj);
    QByteArray cmd = jsonDoc.toJson(QJsonDocument::Compact);
    return cmd;
}

void Slimp3JSON::sendJSONCommand(QString mac, QStringList cmds)
{
    QNetworkRequest req(url);
    DEBUGF(url);

    QByteArray cmd = formatJSONCommand(mac, cmds);
    DEBUGF(url << cmd);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    DEBUGF( req.rawHeaderList() );
    lastRequest.clear();
    lastRequest = cmd;
    mgr->post(req,cmd);
}

void Slimp3JSON::receiveJSONReply(QNetworkReply* reply)
{
    if(reply->error() != QNetworkReply::NoError) {
        QString err = reply->errorString();
        DEBUGF(err);
        DEBUGF(reply->error());
        if( requestCount < 5 ) {
            reply->deleteLater();
            requestCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            QNetworkRequest req(url);
            mgr->post(req, lastRequest);
        }

    } else {
        requestCount = 0;
        QString contents = QString::fromUtf8(reply->readAll());
        QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());

        if( doc["params"].isArray() ) {
            QJsonValue array(doc["params"]);
            DEBUGF("JSON DOC:" << doc << doc["params"].isArray() << array << array[1][0]);

            if( array[0] !="") {
                if( squeezeServer->hasMacAddress( array[0].toString().toLatin1() )) {
                    DEBUGF("Updating mac address:" << array[0].toString());
                    // squeezeServer->UpdatePlayer(array[0].toString(), doc);
                }
            }

            if( array[1][0].toString() == "players" ) {
                DEBUGF("Adding player:" << doc);
                squeezeServer->processPlayerJson( doc[ "result" ] );
                // emit PlayersAdded(doc["result"]);

            } else if( array[1][0] == "status" ) {
                squeezeServer->processStatusJson( doc[ "result" ] );
            }
        }
    }
    reply->deleteLater();
}

void Slimp3JSON::processPlayerInit(QJsonValue jValue)
{
}

void Slimp3JSON::processPlayerMessage(QJsonValue jValue)
{

}
