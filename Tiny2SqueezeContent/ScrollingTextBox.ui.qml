import QtQuick
import QtQuick.Controls

Rectangle {
    id: scrollingTextBox
    width: 200
    height: 50
    color: "lightgray"
    clip: true

    property int textWidth: scrollTextItem.paintedWidth
    property int boxWidth: width
    property string text: "This is a long text example that should scroll when displayed, but if it doesn't you have a problem"
    property int scrollDuration: 3000 // Duration for scrolling
    property int fadeDuration: 1000 // Duration for fading out
    property int resetDelay: 1000 // Delay before resetting
    property int pauseBeforeScroll: 5000 // Initial delay before scrolling
    property color textColor: "black"
    property int leftPadding: 0
    property int rightPadding: 0

    Text {
        id: scrollTextItem
        text: scrollingTextBox.text
        // text: "This is a long text that should scroll when it overflows the box."
        font.pixelSize: 20
        color: scrollingTextBox.textColor
        anchors.verticalCenter: parent.verticalCenter
        x: scrollingTextBox.leftPadding
        clip: true
        // anchors.fill: parent
        anchors.leftMargin: leftPadding
        anchors.rightMargin: rightPadding
    }

    SequentialAnimation {
        id: textScrollAnimation
        running: scrollTextItem.paintedWidth > parent.width
        loops: Animation.Infinite

        PauseAnimation {
            duration: pauseBeforeScroll
        } // Initial delay
        NumberAnimation {
            target: scrollTextItem
            property: "x"
            from: scrollingTextBox.leftPadding
            to: -(textWidth - 5 * boxWidth / 6)
            duration: scrollDuration
            easing.type: Easing.Linear
        }
        OpacityAnimator {
            target: scrollTextItem
            from: 1.0
            to: 0.0
            duration: fadeDuration
        }
        PauseAnimation {
            duration: resetDelay
        }
        PropertyAction {
            target: scrollTextItem
            property: "x"
            value: scrollingTextBox.leftPadding
        }
        OpacityAnimator {
            target: scrollTextItem
            from: 0.0
            to: 1.0
            duration: resetDelay
        }
    }
}
