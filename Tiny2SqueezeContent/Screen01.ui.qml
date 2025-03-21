

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import Tiny2Squeeze

Rectangle {
    id: tinySqueezeScreen
    width: 450
    height: 250

    color: Constants.backgroundColor

    signal setupButtonClicked(bool open)
    signal rewindBtnClicked
    signal pauseBtnClicked
    signal forwardBtnClicked
    signal muteBtnClicked
    signal playerVolumeChanged(int vol)

    property alias artworkID: coverImage.source
    property alias artist: artistRect.text
    property alias album: albumRect.text
    property alias title: titleRect.text
    property bool isMuted: false
    property alias isPaused: pauseBtn.paused
    property alias volume: volumeSlider.value
    property alias songProgress: progressBar.value
    property alias songTimeRemaining: songTimeLabel.text

    ImageButton {
        id: coverImage
        width: 200
        height: 200
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        source: "images/musical_note.png"
        fillMode: Image.PreserveAspectFit
        property bool muted: false

        Connections {
            target: coverImage
            function onImageClicked() {
                tinySqueezeScreen.muteBtnClicked()
            }
        }
    }

    Rectangle {
        id: controlsRect
        height: 70
        color: "#ffffff"
        anchors.left: coverImage.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        property int buttonMargin: 2

        ImageButton {
            id: revBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: controlsRect.buttonMargin
            source: "images/new_reverse.png"
            fillMode: Image.PreserveAspectFit
            height: controlsRect.height - (2 * controlsRect.buttonMargin)
            width: height

            Connections {
                target: revBtn
                function onImageClicked() {
                    tinySqueezeScreen.rewindBtnClicked()
                }
            }
        }

        ImageButton {
            id: pauseBtn
            anchors.verticalCenter: parent.verticalCenter
            source: (paused) ? "images/play.png" : "images/pause.png"
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            height: controlsRect.height - (2 * controlsRect.buttonMargin)
            width: height
            property bool paused: true

            Connections {
                target: pauseBtn
                function onImageClicked() {
                    pauseBtn.paused = !pauseBtn.paused
                    tinySqueezeScreen.pauseBtnClicked()
                }
            }
        }

        ImageButton {
            id: forwardBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: controlsRect.buttonMargin
            source: "images/new_forward.png"
            fillMode: Image.PreserveAspectFit
            height: controlsRect.height - (2 * controlsRect.buttonMargin)
            width: height

            Connections {
                target: forwardBtn
                function onImageClicked() {
                    tinySqueezeScreen.forwardBtnClicked()
                }
            }
        }
    }

    ScrollingTextBox {
        id: titleRect
        color: "#ffffff"
        anchors.left: parent.left
        leftPadding: 5
        rightPadding: 5
        anchors.right: settingsBtn.left
        anchors.top: parent.top
        height: parent.height - coverImage.height
        scrollDuration: 5000
        pauseBeforeScroll: 3000
        fadeDuration: 3000
        resetDelay: 1000
        // text: "Tile of Song To Be PlayedTile of Song To Be PlayedTile of Song To Be PlayedTile of Song To Be Played"
        text: "Tile of Song"
    }

    ScrollingTextBox {
        id: artistRect
        color: "#ffffff"
        anchors.left: coverImage.right
        leftPadding: 5
        anchors.right: parent.right
        anchors.top: titleRect.bottom
        height: (parent.height - (titleRect.height + controlsRect.height)) / 3
        scrollDuration: 5000
        pauseBeforeScroll: 3000
        fadeDuration: 3000
        resetDelay: 1000
        text: "The Name of the Artist"
    }

    ScrollingTextBox {
        id: albumRect
        color: "#ffffff"
        height: artistRect.height
        anchors.left: coverImage.right
        leftPadding: 5
        anchors.right: parent.right
        anchors.top: artistRect.bottom
        scrollDuration: 5000
        pauseBeforeScroll: 3000
        fadeDuration: 3000
        resetDelay: 1000
        text: "The Name of the Ablum"
    }

    ImageButton {
        id: settingsBtn
        Image {
            id: settingsBtnImage
            source: "images/gear.png"
            anchors.fill: parent
            anchors.margins: settingsBtn.width / 5
        }

        Connections {
            target: settingsBtn
            function onImageClicked() {
                tinySqueezeScreen.setupButtonClicked(true)
            }
        }
        anchors.right: parent.right
        anchors.top: parent.top
        height: titleRect.height
        width: height
    }

    ProgressBar {
        id: progressBar
        value: 0.5
        height: (3 * artistRect.height) / 8

        anchors.left: coverImage.right
        anchors.right: parent.right
        anchors.top: albumRect.bottom
    }

    Label {
        id: songTimeLabel
        text: tinySqueezeScreen.songTimeRemaining
        anchors.verticalCenter: progressBar.verticalCenter
        anchors.right: progressBar.right
        anchors.rightMargin: 5
        color: "black"
        z: 10
    }

    Slider {
        id: volumeSlider
        value: 0.5
        anchors.left: coverImage.right
        anchors.right: parent.right
        anchors.top: progressBar.bottom
        anchors.bottom: controlsRect.top
        live: false
        Connections {
            target: volumeSlider
            function onValueChanged() {
                tinySqueezeScreen.playerVolumeChanged(volumeSlider.value * 100)
            }
        }
    }
}
