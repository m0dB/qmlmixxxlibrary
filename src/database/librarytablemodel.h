#pragma once

#include <qqml.h>

#include <QSqlQueryModel>

class LibraryTableModel : public QSqlQueryModel {
    Q_OBJECT
    QML_ELEMENT
    QStringList m_fields;

  public:
    LibraryTableModel(QObject* parent = nullptr);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void sortByColumn(int column, Qt::SortOrder sortOrder);
};
