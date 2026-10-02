import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NomadClient



Item
{
    visible: true
    anchors.fill: parent

    RowLayout
    {
        anchors.fill: parent
        spacing: 0

        Rectangle
        {
            Layout.fillHeight: true
            Layout.preferredWidth: 72
            color: "#1e1f22"
            MainLeftPannel
            {
                anchors.fill: parent
            }
        }

        Rectangle
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#1e1f22"
            StackLayout
            {
                id: mainStackLayout
                anchors.fill: parent
                currentIndex: UIStateManager.State

                // ==========================================================
                // 1. Личное (Друзья и личные чаты)
                // ==========================================================
                Rectangle
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Friends
                    {
                        anchors.fill: parent
                    }
                }

                // ==========================================================
                // 2. Сервера
                // ==========================================================
                Rectangle
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Server
                    {
                        anchors.fill: parent
                    }
                }
            }
        }
    }
}
