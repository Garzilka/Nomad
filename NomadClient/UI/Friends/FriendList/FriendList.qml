import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: friendsListViewRoot
    color: "#313338"

    property string selectedFriendName: ""

    // Внутреннее состояние вкладки для хедера:
    // 0 - Все контакты, 1 - Ожидание (заявки), 2 - Добавить в друзья
    property int internalTab: 0

    Menu
    {
        id: globalContextMenu
        background: Rectangle { implicitWidth: 180; color: "#111214"; border.color: "#1f2023"; border.width: 1; radius: 4 }

        delegate: MenuItem
        {
            id: menuItem
            leftPadding: 12; rightPadding: 12; topPadding: 8; bottomPadding: 8
            contentItem: Text { text: menuItem.text; color: menuItem.hovered ? "#ffffff" : "#b5bac1"; font.pixelSize: 13 }
            background: Rectangle { color: menuItem.hovered ? "#4752c4" : "transparent"; radius: 4; anchors.fill: parent; anchors.margins: 4 }
        }

        Action
        {
            text: "Написать сообщение";
            onTriggered:
            {
                ChatManager.openPrivateChat(friendsMainView.currentItem.modelData["guid"]);
            }
        }
        Action { text: "Удалить из друзей"; onTriggered: { console.log("Удаление: " + friendsListViewRoot.selectedFriendName); } }
        MenuSeparator { contentItem: Rectangle { implicitHeight: 1; color: "#1f2023"; anchors.left: parent ? parent.left : undefined; anchors.right: parent ? parent.right : undefined; anchors.leftMargin: 8; anchors.rightMargin: 8 } }
        Action { text: "Заблокировать"; onTriggered: { console.log("Блокировка: " + friendsListViewRoot.selectedFriendName); } }
    }

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // ==========================================
        // Хедер списка друзей
        // ==========================================
        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: "#313338"
            border.color: "#1f2023"
            border.width: 1

            RowLayout
            {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 16

                Text
                {
                    text: "🧑‍🤝‍🧑 Друзья";
                    color: "#ffffff";
                    font.bold: true;
                    font.pixelSize: 16
                }

                Rectangle { width: 1; height: 16; color: "#3f4147" }

                // ==========================================
                // Вкладка "Все контакты"
                // ==========================================
                Text
                {
                    text: "Все контакты (" + friendsMainView.count + ")";
                    color: friendsListViewRoot.internalTab === 0 ? "#ffffff" : "#b5bac1";
                    font.bold: true;
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: friendsListViewRoot.internalTab = 0
                    }
                }

                // ==========================================
                // Вкладка "Ожидание" (Заявки) внутри хедера
                // ==========================================
                Text
                {
                    text: "Ожидание (" + pendingListView.count + ")";
                    color: friendsListViewRoot.internalTab === 1 ? "#ffffff" : "#b5bac1";
                    font.bold: true;
                    font.pixelSize: 14

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: friendsListViewRoot.internalTab = 1
                    }
                }

                // ==========================================
                // Кнопка "Добавить в друзья"
                // ==========================================
                Button
                {
                    id: addFriendBtn
                    text: "Добавить в друзья"

                    contentItem: Text {
                        text: addFriendBtn.text
                        // Если вкладка активна — текст зеленый, иначе белый
                        color: friendsListViewRoot.internalTab === 2 ? "#23a55a" : "#ffffff"
                        font.bold: true
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        implicitWidth: 140
                        implicitHeight: 28
                        // Если активна — прозрачная кнопка с зеленым текстом, иначе зеленая заливка
                        color: friendsListViewRoot.internalTab === 2 ? "transparent" : "#248046"
                        radius: 4
                    }

                    HoverHandler
                    {
                        cursorShape: Qt.PointingHandCursor
                    }
                    onClicked: friendsListViewRoot.internalTab = 2
                }

                Item { Layout.fillWidth: true }
            }
        }

        // ==========================================
        // Центральный переключатель содержимого вкладки Друзья
        // ==========================================
        StackLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: friendsListViewRoot.internalTab

            // ==========================================
            // Слой 0: Основная лента списка контактов (Все друзья)
            // ==========================================
            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Заглушка, если список пуст
                Text {
                    text: "У вас пока нет друзей. Нажмите кнопку Добавить в друзья!"
                    color: "#949ba4"
                    font.pixelSize: 15
                    anchors.centerIn: parent
                    visible: friendsMainView.count === 0
                }

                ListView
                {
                    id: friendsMainView
                    anchors.fill: parent
                    anchors.margins: 20
                    model: FriendsManager.friendsList
                    spacing: 12
                    clip: true
                    visible: count > 0

                    delegate: FriendListItem
                    {
                        width: friendsMainView.width
                        _avatarColor: modelData["avatarUrl"] ? modelData["avatarUrl"] : "#5865f2"
                        _statusColor: modelData["isOnline"] ? "#23a55a" : "#80848e"
                        _name: modelData["displayName"] ? modelData["displayName"] : ""
                        _status: modelData["isOnline"] ? "В сети" : "Не в сети"
                        _currentActivity: ""

                        onRightClicked: (friendName, sourceItem, mx, my) => {
                            friendsListViewRoot.selectedFriendName = friendName;
                            var mappedPos = sourceItem.mapToItem(friendsListViewRoot, mx, my);
                            globalContextMenu.popup(mappedPos.x, mappedPos.y);
                        }
                    }
                }
            }

            // ==========================================
            // Слой 1: Внутренний контейнер для списка ожидающих заявок
            // ==========================================
            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ListView
                {
                    id: pendingListView
                    anchors.fill: parent
                    model: FriendsManager.pendingRequests
                }
            }

            // ==========================================
            // Слой 2: Внутренний инпут поиска и добавления друга
            // ==========================================
            FriendSearchWidget
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
