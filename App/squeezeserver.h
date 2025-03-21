#ifndef SQUEEZESERVER_H
#define SQUEEZESERVER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QHash>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QTimer>
#include "slimp3cli.h"
#include "slimp3json.h"
// #include "squeezedefines.h"


class SqueezeServer : public QObject
{
    Q_OBJECT
    SqueezeServer(const SqueezeServer &) = delete;
    SqueezeServer(SqueezeServer &&) = delete;
    SqueezeServer &operator=(const SqueezeServer &) = delete;
    SqueezeServer &operator=(SqueezeServer &&) = delete;
    // QML_ELEMENT

    Q_PROPERTY(QString ServerAddress READ ServerAddress CONSTANT)
    Q_PROPERTY(QString cliPort READ cliPort CONSTANT)
    Q_PROPERTY(QString jsonPort READ jsonPort CONSTANT)
    Q_PROPERTY(QHash<QString, QString> players READ players WRITE setPlayers NOTIFY playersChanged FINAL)
    Q_PROPERTY(QHash<QString, QString> player2ID READ player2ID WRITE setPlayer2ID NOTIFY player2IDChanged FINAL)

    Q_PROPERTY(QString activePlayerMac READ activePlayerMac WRITE setActivePlayerMac NOTIFY activePlayerMacChanged FINAL)
    Q_PROPERTY(QString playerMode READ playerMode WRITE setPlayerMode NOTIFY playerModeChanged FINAL)

public:
    explicit SqueezeServer(QObject *parent, QQmlApplicationEngine *myEngine);


    QString ServerAddress() const;
    QString cliPort() const;
    QString jsonPort() const;

    bool hasMacAddress(QString mac);

    void processPlayerJson( QJsonValue result);
    void processStatusJson( QJsonValue result);

    Q_INVOKABLE QHash<QString, QString> players() {return m_id2Player;}
    void setPlayers(const QHash<QString, QString> &newPlayers);

    Q_INVOKABLE QHash<QString, QString> player2ID() { return m_id2Player; }
    void setPlayer2ID(const QHash<QString, QString> &newPlayer2ID);

    Q_INVOKABLE QString getPlayerName(QString mac) {return m_id2Player.value("mac");}
    Q_INVOKABLE QString getPlayerMac(QString name) { return m_player2ID.value("name"); }

    QString activePlayerMac() const;
    void setActivePlayerMac(const QString &newActivePlayerMac);

    void processCLICommand( QString cmd );

    QString playerMode() const;
    void setPlayerMode(const QString &newPlayerMode);

public slots:
    void receiveCLIResponse(QByteArray msg);
    // void receiveJSONResponse(QByteArray msg);

    void init(QString t_serverAddress, QString t_jsonPort, QString t_cliPort);
    void serverConnectionFailed();

    void pauseButton(bool isPaused);
    void forwardButton();
    void rewindButton(bool isSongPlaying);
    void setMuteState( bool state );
    void setVolume( int vol );

    void songChange(void);

    Q_INVOKABLE void setActivePlayer( QString mac );

signals:
    void ServerAddressChanged();
    void cliPortChanged();
    void jsonPortChanged();
    void songChanged();

    void playersChanged();
    void player2IDChanged();

    void serverInitialized( int playerCount );
    void serverInitFailed();

    void playerAdded( QString name, QString mac );

// UI signals
    void setUINowPlaying( QString imageURL, QString songTitle, QString albumTitle, QString artistName );
    void setUIMute( bool muted );
    void setUIPlayMode( QString mode );
    void setUIVolume( int vol );
    void setUISongProgress( double duration, double time );
    void setUISongTime( double time );

    void setCoverImage( QString imageURL );
    void setSongTitle( QString songTitle );
    void setAlbumTitle( QString albumTitle );
    void setArtistName( QString artistName );
    void setUIPauseStatus( bool isPaused );

    void activePlayerMacChanged();

    void playerModeChanged();

private:
    QString m_ServerAddress;
    QString m_cliPort;
    QString m_jsonPort;
    static QRegularExpression macRegex;
    QQmlApplicationEngine *engine;
    bool m_cliConnected;

    Slimp3JSON *m_jsonCX;
    Slimp3CLI *m_cliCX;

    QTimer tock;
    bool m_uiDone = false;
    QHash<QString, QString> m_id2Player;
    QHash<QString, QString> m_player2ID;
    QString m_activePlayerMac;
    QString m_playerMode;
};

#endif // SQUEEZESERVER_H
