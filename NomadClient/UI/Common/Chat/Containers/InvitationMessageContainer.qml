import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Rectangle
{
    id: root
    property string serverName: ""

    // Ограничиваем максимальную ширину карточки инвайта, чтобы она не растягивалась на весь экран
    Layout.maximumWidth: 450
    height: 72
    color: "#2b2d31"
    radius: 8
    border.color: "#1f2023"
    border.width: 1

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // ==========================================
        // Иконка-заглушка сервера
        // ==========================================
        Rectangle
        {
            width: 48
            height: 48
            radius: 12
            color: "#35363c"

            Text {
                text: "🌐"
                font.pixelSize: 22
                anchors.centerIn: parent
            }
        }

        // ==========================================
        // Информация о сервере
        // ==========================================
        ColumnLayout
        {
            Layout.fillWidth: true
            spacing: 2

            Text
            {
                text: "ВАС ПРИГЛАШАЮТ ПРИСОЕДИНИТЬСЯ"
                color: "#949ba4"
                font.pixelSize: 10
                font.bold: true
            }

            Text
            {
                text: root.serverName
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 14
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }

        // ==========================================
        // Кнопка вступления
        // ==========================================
        Button
        {
            id: joinButton
            text: "Принять"

            contentItem: Text
            {
                text: joinButton.text
                color: "white"
                font.bold: true
                font.pixelSize: 13
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle
            {
                implicitWidth: 70
                implicitHeight: 32
                color: joinButton.hovered ? "#248046" : "#23a55a" // Светится зеленым при ховере
                radius: 4
            }

            HoverHandler { cursorShape: Qt.PointingHandCursor }

            onClicked:
            {
                console.log("Попытка вступить на сервер:", root.serverName)
            }
        }
    }
}
