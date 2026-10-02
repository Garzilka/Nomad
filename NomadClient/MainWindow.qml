import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import NomadClient

ApplicationWindow
{
    id: window
    width: 950
    height: 600
    minimumWidth: 500
    minimumHeight: 400
    visible: false
    title: qsTr("Nomad")
    flags: Qt.Window | Qt.FramelessWindowHint

    onVisibleChanged:
    {
        if (visible)
        {
            openAnimation.start();
        }
    }
    Component.onCompleted:
    {
        x = (Screen.width - width) / 2
        y = (Screen.height - height) / 2
    }

    ParallelAnimation
    {
        id: openAnimation
        NumberAnimation { target: windowContent; property: "scale"; from: 0.92; to: 1.0; duration: 350; easing.type: Easing.OutBack }
        NumberAnimation { target: windowContent; property: "opacity"; from: 0.0; to: 1.0; duration: 250; easing.type: Easing.OutCubic }
    }

    SequentialAnimation
    {
        id: stateChangeAnimation

        NumberAnimation
        {
            target: mainStackLayout
            property: "opacity"
            to: 0.0
            duration: 150
            easing.type: Easing.OutCubic
        }

        PropertyAction
        {
            target: mainStackLayout
            property: "currentIndex"
            value: mainStackLayout.pendingIndex
        }
        PropertyAction
        {
            target: mainStackLayout
            property: "yOffset"
            value: 15
        }

        ParallelAnimation
        {
            NumberAnimation
            {
                target: mainStackLayout
                property: "opacity"
                to: 1.0
                duration: 250
                easing.type: Easing.OutQuad
            }
            NumberAnimation
            {
                target: mainStackLayout
                property: "yOffset"
                to: 0
                duration: 250
                easing.type: Easing.OutCubic
            }
        }
    }

    Item
    {
        id: windowContent
        anchors.fill: parent
        transformOrigin: Item.Center
        opacity: 0.0
        scale: 0.92

        TitleBar
        {
            id: titleBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            mainAppWindow: window
        }

        Item
        {
            id: mainStackLayout
            anchors.top: titleBar.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            property int yOffset: 0
            property int currentIndex: ClientConnection.State
            property int pendingIndex: ClientConnection.State

            Connections
            {
                target: ClientConnection
                function onStateChanged()
                {
                    mainStackLayout.pendingIndex = ClientConnection.State;
                    stateChangeAnimation.start();
                }
            }

            // ==========================================================
            // 1. Логин-экран
            // ==========================================================
            LoginScreen
            {
                anchors.fill: parent
                y: mainStackLayout.currentIndex === 0 ? mainStackLayout.yOffset : 0
                // Показываем, если он активен ИЛИ если идет анимация перехода с/на него
                visible: mainStackLayout.currentIndex === 0 || mainStackLayout.pendingIndex === 0
                opacity: mainStackLayout.currentIndex === 0 ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 150 } }
            }

            // ==========================================================
            // 2. Рабочая зона
            // ==========================================================
            MainWorkspace
            {
                anchors.fill: parent
                y: mainStackLayout.currentIndex === 1 ? mainStackLayout.yOffset : 0
                visible: mainStackLayout.currentIndex === 1 || mainStackLayout.pendingIndex === 1
                opacity: mainStackLayout.currentIndex === 1 ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 150 } }
            }
        }
    }

}
