import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Row {
    id: header
    x: -syncView.contentX
    height: 20
    spacing: syncView.columnSpacing

    property TableView syncView
    property font font
 
    function columnWidth(column) {
        return headerRepeater.itemAt(column).width;
    }

    Repeater {
        id: headerRepeater
        model: syncView.model.columnCount()
        property int lastSortedColumn: -1
        Item {
            id: headerItem
            readonly property string headerText: syncView.model.headerData(index, Qt.Horizontal)
            property int sortOrder: Qt.AscendingOrder
            width: headerText === "" ? 0 : splitter.x + 6
            height: parent.height
            visible: width !== 0
            onWidthChanged: syncView.forceLayout()
            Rectangle {
                x: 1
                y: 0
                height: parent.height
                width: parent.width === 0 ? 0 : parent.width - 2
                color: "#303030"
                Text {
                    id: text
                    verticalAlignment: Text.AlignVCenter
                    clip: true
                    leftPadding: 2
                    color: "white"
                    anchors.fill: parent
                    text: headerText
                    font: header.font
                }
                MouseArea {
                    x: 6
                    width: parent.width - 12
                    height: parent.height
                    onClicked: {
                        if (headerRepeater.lastSortedColumn == index) {
                            headerItem.sortOrder = headerItem.sortOrder == Qt.AscendingOrder ? Qt.DescendingOrder : Qt.AscendingOrder
                        }
                        syncView.model.sortByColumn(index, headerItem.sortOrder)
                        headerRepeater.lastSortedColumn = index
                    }
                }
            }
            Item {
                id: splitter
                x: 94
                width: 12
                height: parent.height
                MouseArea {
                    z: 1
                    anchors.fill: parent
                    drag.target: parent
                    drag.axis: Drag.XAxis
                    hoverEnabled: true
                    cursorShape: containsMouse ? Qt.SplitHCursor : Qt.ArrowCursor
                }
            }
        }
    }
}
