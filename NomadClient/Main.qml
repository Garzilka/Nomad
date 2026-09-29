import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

ApplicationWindow
{
    id: window
    width: 640
    height: 480
    minimumWidth: 200
    minimumHeight: 250
    visible: true
    title: qsTr("Nomad")

    // Main content area
    StackLayout
    {
        anchors.fill: parent
        
        // Login Screen
        LoginScreen
        {
            id: loginScreen
            visible: ClientConnection.State === 0
        }
        
        // Main Workspace
        MainWorkspace
        {
            id: mainWorkspace
            visible: ClientConnection.State === 1
        }
    }
}