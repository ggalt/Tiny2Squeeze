#include "squeezeserver.h"
#include <QDebug>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QQmlContext>
#include <QStringList>


#ifdef SQUEEZESERVER_DEBUG
#define DEBUGF(...) qDebug() << this->objectName() << Q_FUNC_INFO << __VA_ARGS__;
#else
#define DEBUGF(...)
#endif

QRegularExpression SqueezeServer::macRegex = QRegularExpression("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");


SqueezeServer::SqueezeServer(QObject *parent, QQmlApplicationEngine *myEngine)
    : QObject{parent}
{
    engine = myEngine;

    tock.setInterval(1000);
    tock.start();
    DEBUGF("create server");
}

void SqueezeServer::init(QString t_serverAddress, QString t_jsonPort, QString t_cliPort)
{
    DEBUGF("SQUEEZESERVER INIT");
    DEBUGF("SQUEEZESERVER INIT");
    m_ServerAddress = t_serverAddress;
    m_jsonPort = t_jsonPort;
    m_cliPort = t_cliPort;

    m_jsonCX = new Slimp3JSON(this, m_ServerAddress, m_jsonPort);
    m_jsonCX->Init();

    m_cliCX = new Slimp3CLI(this, m_ServerAddress, m_cliPort);
    m_cliCX->Init();

    m_jsonCX->sendJSONCommand("", {"players","0","100"});

}

void SqueezeServer::serverConnectionFailed()
{
    // do something!!
    emit serverInitFailed();

}

QString SqueezeServer::ServerAddress() const
{
    return m_ServerAddress;
}

QString SqueezeServer::cliPort() const
{
    return m_cliPort;
}

QString SqueezeServer::jsonPort() const
{
    return m_jsonPort;
}

void SqueezeServer::pauseButton(bool isPaused )
{
    DEBUGF("PAUSEBUTTON PUSHED WITH:" << isPaused);
    m_jsonCX->sendJSONCommand(this->activePlayerMac(), {"pause", isPaused ? "1" : "0"});
}

void SqueezeServer::forwardButton()
{
    DEBUGF("FORWARD BUTTON PUSHED");
    m_jsonCX->sendJSONCommand(this->activePlayerMac(), {"playlist", "index", "+1" });
}

void SqueezeServer::rewindButton(bool isSongPlaying)
{
    DEBUGF("REWIND BUTTON PUSHED ");
    if(isSongPlaying) {
        m_jsonCX->sendJSONCommand(this->activePlayerMac(), {"playlist", "jump", "+0" });    // go to start of song, not previous one
    } else {
        m_jsonCX->sendJSONCommand(this->activePlayerMac(), {"playlist", "jump", "-1" });
    }

}

void SqueezeServer::setMuteState(bool state)
{
    DEBUGF("MUTE STATE SET TO:" << state);
}

void SqueezeServer::setVolume(int vol)
{
    m_jsonCX->sendJSONCommand(this->activePlayerMac(), {"mixer", "volume", QString("%1").arg(vol)});
}

void SqueezeServer::songChange()
{
    // emit this->PlayerModel()->dataChanged(QModelIndex(), QModelIndex());
    emit songChanged();
}

void SqueezeServer::setActivePlayer(QString mac)
{
    DEBUGF(mac);
    setActivePlayerMac(mac);
}

void SqueezeServer::processPlayerJson(QJsonValue result)
{
    int pCount = result["count"].toInt(-1);

    QJsonValue playerLoop( result["players_loop"] );
    for( int c = 0; c < pCount; c++ ) {
        // m_players.insert()
        m_id2Player.insert( playerLoop[ c ][ "playerid" ].toString( nullptr ),
                         playerLoop[ c ][ "name" ].toString( nullptr ) );
        m_player2ID.insert( playerLoop[ c ][ "name" ].toString( nullptr ),
                           playerLoop[ c ][ "playerid" ].toString( nullptr ) );
        emit playerAdded( playerLoop[ c ][ "name" ].toString( nullptr ),
                playerLoop[ c ][ "playerid" ].toString( nullptr ) );
    }
    DEBUGF(m_id2Player);
    emit serverInitialized(this->players().count());
}

void SqueezeServer::processStatusJson(QJsonValue result)
{
    QJsonValue playlistLoop( result["playlist_loop"]);
    DEBUGF( "PLAYLIST LOOP:" << playlistLoop );
    emit setUINowPlaying( playlistLoop[0]["artwork_track_id"].toString(),
                       playlistLoop[0]["title"].toString(),
                       playlistLoop[0]["album"].toString(),
                       playlistLoop[0]["artist"].toString() );
    setPlayerMode(result[ "mode"].toString());
    emit setUIVolume( result[ "mixer volume"].toInt());
    emit setUISongProgress( result[ "duration" ].toDouble(), result[ "time" ].toDouble() );
    DEBUGF("Duration:" << result[ "duration" ].toDouble() << "Song Time:" << result[ "time" ].toDouble() );
}

bool SqueezeServer::hasMacAddress(QString mac)
{
    QString cleanMac;
    if( mac.contains("%") ) {   // uuencoded string
        cleanMac = QByteArray::fromPercentEncoding(mac.first(27).toLatin1());
    } else {
        cleanMac = mac;
    }
    QRegularExpressionMatch match = macRegex.match(cleanMac);
    bool isMatch = match.hasMatch();
    DEBUGF("original:" << mac << "clean:" << cleanMac << "MATCHED?" << isMatch);
    return isMatch;
}


void SqueezeServer::setPlayers(const QHash<QString, QString> &newPlayers)
{
    if (m_id2Player == newPlayers)
        return;
    m_id2Player = newPlayers;
    emit playersChanged();
}

void SqueezeServer::setPlayer2ID(const QHash<QString, QString> &newPlayer2ID)
{
    if (m_player2ID == newPlayer2ID)
        return;
    m_player2ID = newPlayer2ID;
    emit player2IDChanged();
}


void SqueezeServer::receiveCLIResponse(QByteArray msg)
{
    if( hasMacAddress(msg)) {
        // player specific response
        QString mac = QByteArray::fromPercentEncoding( msg.first(27));
        QString cmd = msg.sliced(28).trimmed();
        if( cmd.endsWith('\n') )
            cmd.removeLast();
        if( cmd.contains("%") )
            cmd = QByteArray::fromPercentEncoding(cmd.toLatin1());
        // receivePlayerCLICommand(mac, cmd);
        DEBUGF(cmd);
        DEBUGF(mac);
        DEBUGF(activePlayerMac());
        if( mac == activePlayerMac() ) {
            processCLICommand(cmd);
        }
    } else {
        DEBUGF(msg);
    }

}


QString SqueezeServer::activePlayerMac() const
{
    return m_activePlayerMac;
}

void SqueezeServer::setActivePlayerMac(const QString &newActivePlayerMac)
{
    if (m_activePlayerMac == newActivePlayerMac)
        return;
    m_activePlayerMac = newActivePlayerMac;
    m_jsonCX->sendJSONCommand(this->activePlayerMac(), QStringList({"status","-","1", "tags:J,a,l"}));
    emit activePlayerMacChanged();
}

void SqueezeServer::processCLICommand(QString cmd)
{
    DEBUGF(cmd);
    QStringList cmdTokens = cmd.split(" ");
    if( cmdTokens[0] == "mixer" ) {
        DEBUGF(cmdTokens);
    } else if( cmdTokens[ 0 ] == "playlist" ) {
        if( cmdTokens[ 1 ] == "pause" ) {
            if( cmdTokens[ 2 ] == "1" ) {
                emit setUIPauseStatus( true );
            } else {
                emit setUIPauseStatus( false );
            }
        } else if( cmdTokens[ 1 ] == "stop" ) {
            emit setUIPauseStatus( true );
        } else if( cmdTokens[ 1 ] == "newsong" ) {
            // QStringList jsonCmd = {"status","-","1", "tags:J,a,l"};
            this->m_jsonCX->sendJSONCommand(this->activePlayerMac(), QStringList({"status","-","1", "tags:J,a,l"}));
        }
        DEBUGF(cmdTokens);
    } else if( cmdTokens[ 0 ] == "pause" ) {
        DEBUGF(cmdTokens);
        emit setUIPauseStatus( true );
    } else {
        DEBUGF("Unknown command token");
    }

}

QString SqueezeServer::playerMode() const
{
    return m_playerMode;
}

void SqueezeServer::setPlayerMode(const QString &newPlayerMode)
{
    if( m_playerMode == newPlayerMode )
        return;
    m_playerMode = newPlayerMode;
    emit playerModeChanged();
    emit setUIPlayMode(m_playerMode);
}
