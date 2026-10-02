import QtQuick
import QtQuick.Layouts
import NomadClient

RowLayout
{
    id: root
    width: parent ? parent.width : 0
    spacing: 16
    Layout.alignment: Qt.AlignTop

    // Входные параметры от ListView
    property int currentType: 0
    property string senderName: ""
    property string messageTime: ""
    property color avatarBgColor: "#5865f2"
    property string rawContent: ""

    // ==========================================
    // Аватар (Общий элемент)
    // ==========================================
    Rectangle
    {
        width: 40
        height: 40
        radius: 20
        color: root.avatarBgColor
        Layout.alignment: Qt.AlignTop

        Text
        {
            text: root.senderName.charAt(0).toUpperCase()
            color: "#1e1f22"
            font.bold: true
            font.pixelSize: 16
            anchors.centerIn: parent
        }
    }

    // ==========================================
    // Правая часть сообщения
    // ==========================================
    ColumnLayout
    {
        Layout.fillWidth: true
        spacing: 6

        // Метаданные (Общий элемент)
        RowLayout
        {
            spacing: 8
            Text
            {
                text: root.senderName
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 14
            }
            Text
            {
                text: root.messageTime
                color: "#949ba4"
                font.pixelSize: 11
            }
            Item
            {
                Layout.fillWidth: true
            }
        }

        // --- Переключаемые контейнеры содержимого ---

        // 0 = Text
        TextMessageContainer
        {
            Layout.fillWidth: true
            text: root.rawContent
            visible: root.currentType === 0
        }

        // 4 = InvitationServer
        InvitationMessageContainer
        {
            Layout.fillWidth: true
            serverName: root.rawContent
            visible: root.currentType === 4
        }
    }
}
