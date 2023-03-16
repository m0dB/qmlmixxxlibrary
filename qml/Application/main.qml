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

    Rectangle {
        id: dragImageItem 

        width: dragImageItemText.implicitWidth + 40
        height: 24
        visible: false
        property alias text: dragImageItemText.text
        color: "#101010"
        radius: 5
        Text {
            color: "#ffffff"
            id: dragImageItemText 
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        id: dragItem

        Drag.dragType: Drag.Automatic
        Drag.supportedActions: Qt.CopyAction
        Drag.mimeData: {
            "text/plain": selectionModel.uriListAsString
        }
    }

    ColumnLayout {
        id: topLayout

        spacing: 0
        width: parent.width
        height: parent.height

        RowLayout {
            ColumnLayout {
                spacing: 0
                Rectangle {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 40
                    color: "black"
                    Button {
                        anchors.centerIn: parent
                        text: "add 100 records"
                        onClicked: {
                            libraryTableModel.insertSomething();
                        }
                    }
                }
                DropArea {
                    id: dropArea

                    property string dropped

                    Layout.preferredWidth: 200
                    Layout.fillHeight: true
                    onDropped: (drop) => {
                        dropArea.dropped = drop.text;
                        drop.accept();
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
                Rectangle {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 100
                    color: "black"
                    Rectangle {
                       color: "red"
                       height: 100
                       Timer {
                           interval: 1000/60; running: true; repeat: true
                            onTriggered: {
                                parent.width = (parent.width + 1)%200
                            }
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
                    y: 10
                    showColumns: ["artist", "title", "album", "year", "duration", "bpm", "datetime_added"]
                    implicitHeight: 20
                    model: libraryTableModel
                    columnSpacing: 0 
                    tableViewColumnSpacing: libraryTableView.columnSpacing
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

                        Rectangle {
                            x: 1
                            width: parent.width - 2
                            height: 1
                            color: "#10ffffff"
                        }
                        Rectangle {
                            x: 1
                            y: parent.height - 1
                            width: parent.width - 2
                            height: 1
                            color: "#30000000"
                        }
                        Rectangle {
                            y: 1
                            width: 1
                            height: parent.height - 2 
                            color: "#10ffffff"
                        }
                        Rectangle {
                            x: parent.width - 1
                            y: 1
                            width: 1
                            height: parent.height - 2
                            color: "#30000000"
                        }

                    }

                }
////////////////
                TableView {
                    id: libraryTableView
                    property int dragImageItemCount: 0
                    contentY: libraryTableModel.readyContentY
                    contentHeight: libraryTableModel.totalRowCount * 20
                    interactive: false
                    clip: true
                    y: headerView.y + headerView.height
                    width: parent.width
                    height: parent.height - y
                    model: libraryTableModel
                    columnSpacing: 0
                    columnWidthProvider: headerView.columnWidth
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
                            // Eliding makes layout changes really slow.
                            // Maybe only elide while not changing the column widths?
                            //elide: Text.ElideRight

                            leftPadding: 2
                            color: "white"
                            text: display
                            font.pixelSize: 14
                            width: parent.width
                            height: parent.height
                        }

                        Rectangle {
                            x: parent.width - 4
                            width: 4
                            height: parent.height
                            color: selected ? "#808080" : "#000000"
                        }
                    }
                    reuseItems: true
                }
                ListView {
                    id: listView
                    model: libraryTableModel.totalRowCount
                    clip: true
                    y: headerView.y + headerView.height
                    reuseItems: true
                    width: parent.width
                    height: parent.height - y
                    contentHeight: libraryTableView.contentHeight
                    contentWidth: libraryTableView.contentWidth
                    onContentYChanged: {
                        libraryTableModel.targetContentY = contentY
                    }
                    onContentXChanged: {
                        libraryTableView.contentX = contentX
                    }
                    // avoid scrollbars interfering with selection
                    ScrollBar.horizontal: ScrollBar {
                        interactive: size != 1
                    }

                    ScrollBar.vertical: ScrollBar {
                        interactive: size != 1
                    }
                    delegate: Item {
                        width: listView.width
                        implicitHeight: 20
                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            drag.target: dragItem
                            onPressed: (mouse) => {
                                if (mouse.modifiers & Qt.ControlModifier)
                                    libraryTableView.selectionModel.toggleRow(index);
                                else if (mouse.modifiers & Qt.ShiftModifier)
                                    libraryTableView.selectionModel.selectAdjecentRows(index);
                                else if (!libraryTableView.selectionModel.isRowSelected(index))
                                    libraryTableView.selectionModel.selectSingleRow(index);
                                dragImageItem.text = libraryTableView.selectionModel.uriList.length == 1 ? "1 item" : libraryTableView.selectionModel.uriList.length + " items"
                                dragImageItem.grabToImage(function(result) {
                                    libraryTableView.dragImageItemCount = libraryTableView.dragImageItemCount + 1;
                                    result.saveToFile("/tmp/drag"+libraryTableView.dragImageItemCount+"@2x.png");
                                    dragItem.Drag.imageSource = "file:///tmp/drag"+libraryTableView.dragImageItemCount+"@2x.png";
                                }, Qt.size(dragImageItem.width * 2, dragImageItem.height * 2));
                            }
                            onReleased: (mouse) => {
                                if (!(mouse.modifiers & Qt.ControlModifier) && !(mouse.modifiers & Qt.ShiftModifier))
                                    libraryTableView.selectionModel.selectSingleRow(index);

                                dragItem.Drag.drop();
                                dragItem.Drag.active = false;
                            }

                            drag {
                                onActiveChanged: {
                                    dragItem.Drag.active = drag.active;
                                }
                            }

                        }
                    }
                }
            }

        }

    }

}
