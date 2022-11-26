import Database
import Extensions
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
            ColumnLayout {
                Button {
                    text: "add 100 records"
                    onClicked: {
                        libraryTableModel.insertSomething();
                    }
                }

                DropArea {
                    id: dropArea

                    property string dropped

                    Layout.preferredWidth: 200
                    Layout.fillHeight: true
                    onDropped: (drop) => {
                        console.log(drop, drop.text, drop.urls);
                        dropArea.dropped = drop.urls[0];
                        drop.accepted = true;
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: "#303030"

                        Text {
                            anchors.fill: parent
                            wrapMode: Text.WrapAnywhere
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "#ffffff"
                            text: "drop:\n" + dropArea.dropped
                        }

                    }

                }

            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                HeaderView {
                    id: headerView

                    x: -libraryTableView.contentX
                    showColumns: ["artist", "title", "album", "year", "duration", "bpm", "datetime_added"]
                    implicitHeight: 20
                    model: libraryTableModel
                    columnSpacing: 2
                    onWidthChanged: {
                        libraryTableView.forceLayout();
                        libraryTableView.contentWidth = width;
                    }
                    onSort: (column, order) => {
                        libraryTableModel.sort(column, order);
                    }

                    delegate: Rectangle {
                        required property bool isSorting
                        required property string label
                        required property int sortOrder

                        clip: true
                        color: isSorting ? "#404040" : "#202020"

                        Text {
                            leftPadding: 2
                            text: label + (isSorting ? sortOrder == Qt.AscendingOrder ? " ^" : " v" : "")
                            color: "white"
                            anchors.fill: parent
                        }

                    }

                }

                TableView {
                    id: libraryTableView

                    contentHeight: libraryTableModel.totalRowCount * 20
                    clip: true
                    y: headerView.height
                    width: parent.width
                    height: parent.height - headerView.height
                    model: libraryTableModel
                    columnSpacing: 2
                    columnWidthProvider: headerView.columnWidth

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

                    delegate: Rectangle {
                        required property bool selected

                        implicitWidth: 1
                        implicitHeight: 20
                        color: selected ? "#808080" : "#000000"
                        clip: true

                        Text {
                            //elide: Text.ElideRight

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
                                "text/plain": "some tracks",
                                "text/uri-list": "mixxx:://rows/" + selectionModel.dragString
                            }
                            Drag.onDragFinished: (dropAction) => {
                                console.log("drag finished");
                            }

                            // avoid unexpected binding loop
                            Binding on Drag.active {
                                value: mouseArea.drag.active && selected
                                delayed: true
                            }

                        }

                        MouseArea {
                            // we might start dragging so postpone select single row
                            // click without drag

                            id: mouseArea

                            anchors.fill: parent
                            drag.target: draggable
                            onPressed: (mouse) => {
                                if (mouse.modifiers & Qt.ControlModifier)
                                    selectionModel.toggleRow(row);
                                else if (mouse.modifiers & Qt.ShiftModifier)
                                    selectionModel.selectAdjecentRows(row);
                                else if (!selected)
                                    selectionModel.selectSingleRow(row);
                            }
                            onReleased: (mouse) => {
                                if (!(mouse.modifiers & Qt.ControlModifier) && !(mouse.modifiers & Qt.ShiftModifier))
                                    selectionModel.selectSingleRow(row);

                                draggable.Drag.drop();
                            }
                        }

                    }

                }

            }

        }

    }

}
