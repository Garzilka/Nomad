import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NomadClient

Item
{
    anchors.fill: parent
    RowLayout
    {
        anchors.fill: parent
        spacing: 0
        Rectangle
        {
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            color: "#1e1f22"

            ServerPannel { }
        }

        Rectangle
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#2b2d31"

            ServerWorkSpace {}
        }
    }
}
