import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import seasmoke

ApplicationWindow {
    id: window

    width: 720
    height: 820
    minimumWidth: 360
    minimumHeight: 480
    visible: true
    title: qsTr("SeaSmoke")

    // 0 = system, 1 = light, 2 = dark
    property int themeMode: 0

    Material.theme: themeMode === 1 ? Material.Light
                  : themeMode === 2 ? Material.Dark
                                    : Material.System
    Material.accent: Material.Blue
    Material.primary: Material.Blue

    color: Material.background

    readonly property bool isLight: Material.theme === Material.Light
            || (Material.theme === Material.System
                && Application.styleHints.colorScheme === Qt.Light)

    ScrollView {
        id: scroll
        anchors.fill: parent
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: scroll.availableWidth
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: hero.implicitHeight + 48

                Rectangle {
                    anchors.fill: parent
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: window.isLight
                                   ? Material.color(Material.Blue, Material.Shade100)
                                   : Material.color(Material.BlueGrey, Material.Shade900)
                        }
                        GradientStop {
                            position: 1.0
                            color: window.Material.background
                        }
                    }
                }

                ColumnLayout {
                    id: hero
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 36
                    width: Math.min(560, parent.width - 48)
                    spacing: 12

                    Label {
                        text: qsTr("SeaSmoke")
                        color: window.Material.accent
                        font.pixelSize: 14
                        font.bold: true
                        font.letterSpacing: 1.2
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Label {
                        text: qsTr("Hello, Qt Quick")
                        color: window.Material.foreground
                        font.pixelSize: 34
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    Label {
                        text: qsTr("Qt desktop client for the Axum API at %1").arg(AppViewModel.baseUrl)
                        opacity: 0.7
                        color: window.Material.foreground
                        font.pixelSize: 15
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 8
                        spacing: 10

                        Button {
                            text: window.isLight ? qsTr("Dark mode") : qsTr("Light mode")
                            flat: true
                            Material.roundedScale: Material.SmallScale
                            onClicked: window.themeMode = window.isLight ? 2 : 1
                        }

                        Button {
                            text: qsTr("System")
                            flat: true
                            enabled: window.themeMode !== 0
                            Material.roundedScale: Material.SmallScale
                            onClicked: window.themeMode = 0
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 4
                        textFormat: Text.RichText
                        text: qsTr("API health: <b>%1</b>").arg(AppViewModel.health)
                        color: window.Material.foreground
                        font.pixelSize: 14
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.leftMargin: 24
                Layout.rightMargin: 24
                Layout.bottomMargin: 32
                width: Math.min(560, scroll.availableWidth - 48)
                spacing: 16

                Frame {
                    Layout.fillWidth: true
                    Material.elevation: 1
                    Material.roundedScale: Material.MediumScale

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10

                        Label {
                            text: qsTr("Fan-out demo")
                            color: window.Material.foreground
                            font.pixelSize: 20
                            font.bold: true
                        }

                        Label {
                            text: qsTr("tokio::spawn workers (Go goroutine analogue): fan-out / fan-in")
                            opacity: 0.7
                            color: window.Material.foreground
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Button {
                            text: AppViewModel.starting ? qsTr("Starting...") : qsTr("Run 8 workers")
                            enabled: !AppViewModel.starting
                            highlighted: true
                            Layout.topMargin: 4
                            Material.roundedScale: Material.SmallScale
                            onClicked: AppViewModel.runFanout(8)
                        }

                        Label {
                            visible: AppViewModel.jobs.count === 0
                            text: qsTr("No jobs yet")
                            opacity: 0.7
                            color: window.Material.foreground
                            font.pixelSize: 13
                            Layout.topMargin: 4
                        }

                        Repeater {
                            model: AppViewModel.jobs

                            ColumnLayout {
                                required property string summary
                                required property var workers
                                required property bool isRunning

                                Layout.fillWidth: true
                                spacing: 4

                                Label {
                                    text: summary
                                    color: window.Material.foreground
                                    font.pixelSize: 13
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                }

                                Pane {
                                    Layout.fillWidth: true
                                    padding: 8
                                    Material.elevation: 0
                                    Material.background: window.isLight
                                            ? Material.color(Material.Grey, Material.Shade100)
                                            : Material.color(Material.Grey, Material.Shade900)
                                    Material.roundedScale: Material.SmallScale

                                    ColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        spacing: 2

                                        Label {
                                            visible: isRunning && workers.length === 0
                                            text: qsTr("running...")
                                            opacity: 0.7
                                            color: window.Material.foreground
                                            font.pixelSize: 12
                                            font.family: "Consolas"
                                        }

                                        Repeater {
                                            model: workers

                                            Label {
                                                required property var modelData
                                                text: qsTr("worker %1: result=%2 %3ms")
                                                    .arg(modelData.id)
                                                    .arg(modelData.result)
                                                    .arg(modelData.elapsedMs)
                                                color: window.Material.foreground
                                                font.pixelSize: 12
                                                font.family: "Consolas"
                                                Layout.fillWidth: true
                                                wrapMode: Text.WordWrap
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Frame {
                    Layout.fillWidth: true
                    Material.elevation: 1
                    Material.roundedScale: Material.MediumScale

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10

                        Label {
                            text: qsTr("Recent requests")
                            color: window.Material.foreground
                            font.pixelSize: 20
                            font.bold: true
                        }

                        Label {
                            text: qsTr("Logged by Axum middleware (API routes only)")
                            opacity: 0.7
                            color: window.Material.foreground
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Label {
                            visible: AppViewModel.requests.count === 0
                            text: qsTr("No API requests yet")
                            opacity: 0.7
                            color: window.Material.foreground
                            font.pixelSize: 13
                        }

                        Pane {
                            visible: AppViewModel.requests.count > 0
                            Layout.fillWidth: true
                            padding: 8
                            Material.elevation: 0
                            Material.background: window.isLight
                                    ? Material.color(Material.Grey, Material.Shade100)
                                    : Material.color(Material.Grey, Material.Shade900)
                            Material.roundedScale: Material.SmallScale

                            ColumnLayout {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                spacing: 2

                                Repeater {
                                    model: AppViewModel.requests

                                    Label {
                                        required property string line
                                        text: line
                                        color: window.Material.foreground
                                        font.pixelSize: 12
                                        font.family: "Consolas"
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
