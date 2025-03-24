// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtCore
import Tiny2Squeeze
import com.georgegalt.SqueezeServer

Window {
    id: mainWindow
    width: mainScreen.width
    height: mainScreen.height

    visible: true
    title: "Tiny2Squeeze - "+ currentPlayerName

    property string serverIP: "127.0.0.1"
    property string cliPort: "9090"
    property string jsonPort: "9000"
    property string currentPlayerName: ""
    property string currentPlayerMac: ""

    property alias playerList: setupDrawer.playerList
    property real songDuration: 0.01    // avoid a divide by zero error
    property real songTime: 0.0

    Dialog {
        id: warningDialog
        title: "Server Connection Failed"
        modal: true
        width: 4*mainWindow.width/5
        height: 4*mainWindow.height/5

        contentItem: Text {
            text: "Do you want to proceed?"
            horizontalAlignment: Text.AlignHCenter
        }

        DialogButtonBox {
            standardButtons: DialogButtonBox.Retry | DialogButtonBox.Abort

            onAccepted: console.log("Retry clicked")
            onRejected: console.log("Abort clicked")
        }

        // onAccepted: console.log("User accepted")
        // onRejected: console.log("User canceled")

    }


    signal initializeServer(string m_serverIP, string m_jsonPort, string m_cliPort)

    function setupServer() {
        SqueezeServer.init(mainWindow.serverIP, mainWindow.jsonPort, mainWindow.cliPort);
        // console.log("starting up server", mainWindow.serverIP, mainWindow.jsonPort, mainWindow.cliPort)
    }

    Timer {
        id: songTimer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            songTime = songTime + 1.0
            mainScreen.songProgress = songTime / songDuration

            function formatTime(totalSeconds) {
                if( totalSeconds < -1 ) {
                    songTimer.running = false
                    return "00:00"
                }

                const minutes = Math.floor(totalSeconds / 60);
                const seconds = Math.floor(totalSeconds % 60);
                return `${minutes}:${seconds.toString().padStart(2, '0')}`;
            }
            mainScreen.songTimeRemaining = formatTime(songDuration - songTime)
            // console.log(songTime, songDuration, songTime / songDuration)
            // var timeleft = parseInt(songDuration) - parseInt(songTime)
            // console.log(timeleft/60,":",timeleft%60)
        }
    }

    Settings {
        id: serverSettings
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height

        property alias serverIP: mainWindow.serverIP
        property alias cliPort: mainWindow.cliPort
        property alias jsonPort: mainWindow.jsonPort

        property alias currentPlayerName: mainWindow.currentPlayerName
        property alias currentPlayerMac: mainWindow.currentPlayerMac
    }

    function openDrawer() {
        // console.log("this is opening the drawer")
        drawer.open()
    }

    /// slots for catching signals from server
    Connections {
        target: SqueezeServer
        onServerInitFailed: {
            warningDialog.open()
            // mainWindow.openDrawer()
        }
    }

    Connections {
        target: SqueezeServer
        function onSetUINowPlaying(imageURL, songTitle, albumTitle, artistName) {
            // console.log("caught by app:", imageURL, songTitle, albumTitle, artistName)
            mainScreen.artist = artistName
            mainScreen.album = albumTitle
            mainScreen.title = songTitle
            mainScreen.artworkID = "http://"+mainWindow.serverIP+":"+mainWindow.jsonPort+"/music/"+imageURL+"/cover.jpg"
            // songTimer.restart()
            // if( mainScreen.isPaused ) {
            //     songTimer.stop()
            // }
            // console.log(mainScreen.artworkID)
        }
    }

    Connections {
        target: SqueezeServer
        function onServerInitialized( playerCount ) {
            // console.log("SERVER INITIALIZED")
            if( currentPlayerMac.length > 0 ) {
                SqueezeServer.setActivePlayer( mainWindow.currentPlayerMac )
            }
        }
    }

    Connections {
        target: SqueezeServer
        function onPlayerAdded( pName, mac ) {
            var playerObj = pName+":"+mac
            mainWindow.playerList.append( { key:mac, value:pName })
            // console.log("player:", playerObj)
        }
    }

    Connections {
        target: SqueezeServer
        function onSetUIMute( isMuted ) {
            // console.log("mute")
        }
    }

    Connections {
        target: SqueezeServer
        function onSetUIPlayMode( mode ) {
            // console.log("Player Mode:", mode)
            if( mode === "stop" || mode === "pause" ) {
                mainScreen.isPaused = true
                songTimer.stop()
            } else {
                mainScreen.isPaused = false
                songTimer.start()
            }
        }
    }

    Connections {
        target: SqueezeServer
        function onSetUIVolume( vol ) {
            mainScreen.volume = vol/100.00
            // console.log("Volume is:", vol, (vol/100.00))
        }
    }

    Connections {
        target: SqueezeServer
        function onSetUISongProgress( songDuration, songTime ) {
            mainWindow.songDuration = songDuration
            mainWindow.songTime = songTime
            mainScreen.songProgress = mainWindow.songTime / mainWindow.songDuration
            // songTimer.restart()
            if( mainScreen.isPaused ) {
                songTimer.stop()
            } else {
                songTimer.start()
            }
            console.log("song duration:", mainWindow.songDuration, "song time:", mainWindow.songTime)
        }
    }

    Screen01 {
        id: mainScreen
        Connections {
            target: mainScreen
            function onSetupButtonClicked( val ) {
                mainWindow.openDrawer()
            }
        }
        // Component.onCompleted: {
        //     // set up server

        // }

        Component.onDestruction: serverSettings.sync()
    }

    Connections {
        target: mainScreen
        function onPauseBtnClicked() {
            SqueezeServer.pauseButton( mainScreen.isPaused)
            // console.log("pause button clicked:", "paused is:", mainScreen.isPaused )
        }
    }

    Connections {
        target: mainScreen
        function onForwardBtnClicked() {
            SqueezeServer.forwardButton()
        }
    }

    Connections {
        target: mainScreen
        function onRewindBtnClicked() {
            SqueezeServer.rewindButton( (songTime > 0) ? 1 : 0)
        }
    }

    Connections {
        target: mainScreen
        function onPlayerVolumeChanged( vol ) {
            SqueezeServer.setVolume( vol )
        }
    }

    Drawer {
        id: drawer
        width: mainWindow.width * 0.66
        height: mainWindow.height
        edge: Qt.RightEdge

        MyDrawer {
            id: setupDrawer
            anchors.fill: parent

            serverIP: mainWindow.serverIP
            jsonPort: mainWindow.jsonPort
            cliPort: mainWindow.cliPort

            onDrawerClose: {
                if( setupDrawer.saveInfo === true) {
                    // console.log("true")
                    mainWindow.serverIP = setupDrawer.serverIP
                    mainWindow.cliPort = setupDrawer.cliPort
                    mainWindow.jsonPort = setupDrawer.jsonPort
                    mainWindow.currentPlayerName = setupDrawer.playerName
                    mainWindow.currentPlayerMac = setupDrawer.playerList.get(setupDrawer.playerIndex).key
                    // console.log("selected player:", mainWindow.currentPlayerName, mainWindow.currentPlayerMac)
                    // console.log("Server IP:", mainWindow.serverIP)
                } else {
                    // console.log("false", setupDrawer.saveInfo)
                }
                drawer.close()
            }
        }

    }
}

