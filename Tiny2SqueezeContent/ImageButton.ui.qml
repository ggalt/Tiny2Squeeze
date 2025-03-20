import QtQuick

Image {
    id: imageButton
    opacity: 1.0
    signal imageClicked

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: imageButton.opacity = 0.5
        onReleased: imageButton.opacity = 1.0
        Connections {
            function onClicked() {
                imageButton.imageClicked()
            }
        }

        // onClicked: {
        //     // console.log("image button clicked")
        //     imageButton.imageClicked()
        // }
    }
}
