#ifndef SLIMP3JSON_H
#define SLIMP3JSON_H

#include <QObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "squeezedefines.h"

class Slimp3JSON : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString ServerAddress READ ServerAddress WRITE setServerAddress NOTIFY ServerAddressChanged FINAL)
    Q_PROPERTY(QString JSONPort READ JSONPort WRITE setJSONPort NOTIFY JSONPortChanged FINAL)

public:
    explicit Slimp3JSON(QObject *parent = nullptr,
                        QString serverAddress = "127.0.0.1",
                        QString jsonPort = "9000");

    void Init(void);

    QString ServerAddress() const;
    void setServerAddress(const QString &newServerAddress);

    QString JSONPort() const;
    void setJSONPort(QString newJSONPort);

    void sendJSONCommand(QString mac, QStringList cmds);

public slots:
    void receiveJSONReply(QNetworkReply* reply);

signals:
    void ServerAddressChanged();
    void JSONPortChanged();

private:
    void processPlayerInit( QJsonValue jValue);
    void processPlayerMessage(QJsonValue jValue);
    QByteArray formatJSONCommand(QString mac, QStringList cmds);

private:
    SqueezeServer *squeezeServer;   // convenience pointer
    QString m_ServerAddress;
    QString m_JSONPort;
    QJsonDocument doc;
    QNetworkAccessManager *mgr;
    QUrl url;
    QByteArray lastRequest;
    int requestCount;
};



// curl -g -X POST -d  '{"id":1,"method":"slim.request","params":["00:04:20:04:0b:2c",["players"]]}'
// curl -g -X POST -d  '{"id":1,"method":"slim.request","params":["",["artists","0","20","role_id:ALBUMARTIST","tags:s"]]}' http://192.168.1.50:9000/jsonrpc.js == return artist with textkey
// curl -g -X POST -d  '{"id":1,"method":"slim.request","params":["",["albums","0","20","tags:l,s,j"]]}' http://192.168.1.50:9000/jsonrpc.js                    == return albums with title, textkey, artwork
// curl -g -X POST -d  '{"id":1,"method":"slim.request","params":["",["albums","0","20","artist_id:6572","tags:l,s,j"]]}' http://192.168.1.50:9000/jsonrpc.js   == return albums by artist_id:6572 with title, textkey and artwork
// curl -g -X POST -d '{"id":1,"method":"slim.request","params":["",["genres","0","20","tags:s"]]}' http://192.168.1.50:9000/jsonrpc.js                         == return genres with textkey
// curl -g -X POST -d '{"id":1,"method":"slim.request","params":["",["albums","0","20","genre_id:334","tags:l,s,j"]]}' http://192.168.1.50:9000/jsonrpc.js      == return ablums under genre_id:334 with album, textkey and artwork.



#endif // SLIMP3JSON_H
