import QtQuick
import QtQuick.Controls.Basic

Rectangle
{
    id: root
    color: "#313338"
    clip: true

    ListView
    {
        id: messagesListView
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        clip: true
        model: ChatManager.currentMessages

        delegate: ChatMessageContainer
        {
            currentType: modelData["msgType"] !== undefined ? modelData["msgType"] : 0
            senderName: modelData["sender"] ? modelData["sender"] : ""
            messageTime: modelData["time"] ? modelData["time"] : ""
            avatarBgColor: modelData["avatarColor"] ? modelData["avatarUrl"] : "#5865f2"
            rawContent: modelData["messageText"] ? modelData["messageText"] : ""
        }

        // Автоматический скролл вниз при добавлении новых сообщений
        onCountChanged:
        {
            Qt.callLater(messagesListView.positionViewAtEnd)
        }
    }
}
