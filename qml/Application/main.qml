import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Extensions
import Database 

Window {
    id: root

    width: 800
    height: 400
    visible: true
    color: "#000000"

    LibraryTableModel {
        id: libraryTableModel
    }

    ColumnLayout {
        spacing: 0
        width: parent.width
        height: parent.height

        RowLayout {
            Rectangle {
                Layout.preferredWidth: 200
                Layout.fillHeight: true
                color: "blue"
            }
            ColumnLayout {
                ResizableHeaderView {
                    id: headerView
                    syncView: libraryTableView
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
                TableView {
                    id: libraryTableView
                    clip: true
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: libraryTableModel
                    // avoid scrollbars interfering with selection
                    ScrollBar.horizontal: ScrollBar {
                        interactive: size != 1
                    }
                    ScrollBar.vertical: ScrollBar {
                        interactive: size != 1
                    }
                    selectionModel: MultiRowSelectionModel {
                        id: selectionModel
                        model: libraryTableModel
                    }
        
                    columnSpacing: 0
                    columnWidthProvider: headerView.columnWidth
                    delegate: Rectangle {
                        required property bool selected
                        implicitWidth: 1
                        implicitHeight: 20
                        color: selected ? "#808080" : "#000000"
                        clip: true
                        Text {
                            leftPadding: 2
                            color: "white"
                            text: display
                            font.pixelSize: 14
                            anchors.fill: parent
                        }
                        Item {
                            id: draggable
                            anchors.fill: parent
                            Drag.hotSpot.x: 0
                            Drag.hotSpot.y: 0
                            Drag.dragType: Drag.Automatic
                            Drag.mimeData: {
                            "text/plain": selectionModel.dragString
                            }
                            Drag.onDragFinished: (dropAction) => { }
                            // avoid unexpected binding loop 
                            Binding on Drag.active {
                            value: mouseArea.drag.active && selected
                            delayed: true
                            }
                        }
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            drag.target: draggable
                            onPressed: (mouse) => {
                                if (mouse.modifiers & Qt.ControlModifier) {
                                    selectionModel.toggleRow(row);
                                } else if (mouse.modifiers & Qt.ShiftModifier) {
                                    selectionModel.selectAdjecentRows(row);
                                } else if (!selected) {
                                    // we might start dragging so postpone select single row
                                    selectionModel.selectSingleRow(row);
                                }
                            }
                            onReleased: (mouse) => {
                                if (!(mouse.modifiers & Qt.ControlModifier) &&
                                    !(mouse.modifiers & Qt.ShiftModifier)) {
                                    // click without drag 
                                    selectionModel.selectSingleRow(row);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
