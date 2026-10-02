import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Item
{
    id: leftPanelRoot
    anchors.fill: parent

    ColumnLayout
    {
        anchors.fill: parent
        anchors.topMargin: 12
        spacing: 12

        // ==========================================
        // ВЕРХНЯЯ КНОПКА: ЛИЧНОЕ ПРОСТРАНСТВО (ДРУЗЬЯ)
        // ==========================================
        Button
        {
            id: personalButton
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 48
            implicitHeight: 48
            flat: true

            HoverHandler
            {
                id: personalHover;
                cursorShape: Qt.PointingHandCursor
            }

            background: Rectangle
            {
                color: UIStateManager.State === 0 ? "#5865f2" : (personalButton.hovered ? "#35373c" : "#313338")
                radius: UIStateManager.State === 0 ? 16 : (personalButton.hovered ? 16 : 24)

                Behavior on radius { NumberAnimation { duration: 100 } }
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            contentItem: Text
            {
                text: "💼"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: UIStateManager.State = 0

            ToolTip.visible: hovered
            ToolTip.text: "Личные сообщения и друзья"
        }

        // ==========================================
        // Разделительная линия
        // ==========================================
        Rectangle
        {
            Layout.alignment: Qt.AlignHCenter
            width: 32
            height: 2
            color: "#35363c"
        }

        // ==========================================
        // СПИСОК СЕРВЕРОВ (ОТДЕЛОВ)
        // ==========================================
        Repeater
        {
            model: ["IT", "HR", "MKT"]

            delegate: Button
            {
                id: serverButton
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: 48
                implicitHeight: 48
                flat: true

                HoverHandler { cursorShape: Qt.PointingHandCursor }

                background: Rectangle
                {
                    // Если выбран режим сервера (State === 1), можно дополнительно подсвечивать активный
                    color: UIStateManager.State === 1 ? "#35373c" : (serverButton.hovered ? "#5865f2" : "#313338")
                    radius: serverButton.hovered ? 16 : 24

                    Behavior on radius { NumberAnimation { duration: 100 } }
                    Behavior on color { ColorAnimation { duration: 100 } }
                }

                contentItem: Text
                {
                    text: modelData
                    color: "white"
                    font.bold: true
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: UIStateManager.State = 1

                ToolTip.visible: hovered
                ToolTip.text: "Сервер: " + modelData
            }
        }

        Item
        {
            Layout.fillHeight: true
        }
    }
}
