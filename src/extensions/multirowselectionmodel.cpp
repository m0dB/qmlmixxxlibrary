#include "multirowselectionmodel.h"

#include <algorithm>

MultiRowSelectionModel::MultiRowSelectionModel(QAbstractItemModel* model, QObject* parent)
        : QItemSelectionModel(model, parent) {
}

MultiRowSelectionModel::MultiRowSelectionModel(QAbstractItemModel* model)
        : QItemSelectionModel(model) {
}

void MultiRowSelectionModel::selectSingleRow(int row) {
    select(QItemSelection(model()->index(row, 0), model()->index(row, model()->columnCount() - 1)), QItemSelectionModel::ClearAndSelect);
    m_lastRow = m_adjecentStartRow = row;

    updateUriList();
}

void MultiRowSelectionModel::toggleRow(int row) {
    select(QItemSelection(model()->index(row, 0), model()->index(row, model()->columnCount() - 1)), QItemSelectionModel::Toggle);
    m_lastRow = m_adjecentStartRow = row;

    updateUriList();
}

bool MultiRowSelectionModel::isRowSelected(int row) const {
    return isSelected(model()->index(row, 0));
}

void MultiRowSelectionModel::selectAdjecentRows(int row) {
    // this mimicks the macOS adjecent multi selection behaviour, as observed for example in Mail.app
    // (shift click)
    const int rowCount = model()->rowCount();
    int j = m_adjecentStartRow;
    while (j != rowCount && !isRowSelected(j)) {
        j++;
    }
    if (j == rowCount) {
        j = m_adjecentStartRow;
        while (j != 0 && !isRowSelected(j)) {
            j--;
        }
    }
    int fromRow = std::min(row, j);
    int toRow = std::max(row, j);
    select(QItemSelection(model()->index(fromRow, 0), model()->index(toRow, model()->columnCount() - 1)), QItemSelectionModel::Select);
    if (m_lastRow > toRow || m_lastRow < fromRow) {
        if (m_lastRow > toRow) {
            fromRow = toRow + 1;
            toRow = m_lastRow;
        } else {
            toRow = fromRow - 1;
            fromRow = m_lastRow;
        }
        select(QItemSelection(model()->index(fromRow, 0), model()->index(toRow, model()->columnCount() - 1)), QItemSelectionModel::Deselect);
    }
    m_lastRow = row;

    updateUriList();
}

void MultiRowSelectionModel::updateUriList() {
    m_uriList.clear();
    int column = 0; // location
    QString header = model()->headerData(column, Qt::Horizontal).toString();
    const auto list = selectedRows(column);
    for (const auto index : list) {
        const auto variant = model()->data(index);

        QUrl url;
        url.setScheme("mixxx");
        url.setHost("");
        url.setPath("/" + header + "/" + variant.toString());

        m_uriList.append(url);
    }

    m_uriListAsString.clear();
    for (const auto& url : m_uriList) {
        m_uriListAsString += url.toString() + "\n";
    }

    emit uriListChanged();
}

QList<QUrl> MultiRowSelectionModel::uriList() const {
    return m_uriList;
}

QString MultiRowSelectionModel::uriListAsString() const {
    return m_uriListAsString;
}
