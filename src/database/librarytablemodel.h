#pragma once

#include <qqml.h>

#include <QSortFilterProxyModel>
#include <QSqlTableModel>

class Formatter {
  public:
    virtual ~Formatter() {
    }
    virtual QVariant format(const QVariant& variant) = 0;
};

class LibraryTableModel : public QSortFilterProxyModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int totalRowCount READ totalRowCount NOTIFY totalRowCountChanged)

    class SqlTableModel : public QSqlTableModel {
      public:
        std::vector<std::unique_ptr<Formatter>> m_formatters;

        SqlTableModel(QObject* parent = nullptr);
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    };

    int m_totalRowCount{};
    SqlTableModel* m_sqlTableModel;

    int totalRowCount() {
        return m_totalRowCount;
    }

  signals:
    void totalRowCountChanged();

  public:
    LibraryTableModel(QObject* parent = nullptr);
    Q_INVOKABLE void sort(int column, Qt::SortOrder sortOrder);
    Q_INVOKABLE void insertSomething();
};
