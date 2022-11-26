#include "librarytablemodel.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "mixxxdb.h"

// QSqlQueryModel to retreive the library table from mixxxdb.sqlite
//
// TODO reuse code from mixxx/src/library/basetracktablemodel.cpp
// from column cache, formatting of values, etc
// TODO add sorting per column (asc/dec)

LibraryTableModel::LibraryTableModel(QObject* parent)
        : QSortFilterProxyModel(parent) {
    m_sqlTableModel = new SqlTableModel(parent);
    setSourceModel(m_sqlTableModel);

    QSqlQuery query("SELECT COUNT(*) FROM library", MixxxDb::singleton()->database());
    query.exec();
    if (query.first()) {
        m_totalRowCount = query.value(0).toInt();
    }
}

void LibraryTableModel::insertSomething() {
    for (int i = 0; i < 100; i++) {
        static int k = 0;
        k++;
        QSqlRecord record = m_sqlTableModel->record();
        record.setValue("artist", QString("Artist") + QString::number(k));
        record.setValue("title", QString("Title") + QString::number(k));
        bool result = m_sqlTableModel->insertRecord(-1, record);
        m_sqlTableModel->submitAll();
        m_totalRowCount++;
        emit totalRowCountChanged();
    }
}

void LibraryTableModel::sort(int column, Qt::SortOrder sortOrder) {
    QSortFilterProxyModel::sort(column, sortOrder);
}

class DurationFormatter : public Formatter {
  public:
    QVariant format(const QVariant& variant) {
        if (variant.canConvert<double>()) {
            int seconds = static_cast<int>(variant.toDouble() + 0.5);
            int minutes = seconds / 60;
            seconds -= minutes * 60;
            return QString::number(minutes) + ":" + QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0'));
        }
        return variant;
    }
};

class BpmFormatter : public Formatter {
  public:
    QVariant format(const QVariant& variant) {
        if (variant.canConvert<double>()) {
            return QString::number(variant.toDouble(), 'f', 1);
        }
        return variant;
    }
};

LibraryTableModel::SqlTableModel::SqlTableModel(QObject* parent)
        : QSqlTableModel(parent, MixxxDb::singleton()->database()) {
    setTable("library");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    m_formatters.resize(columnCount());
    for (int i = 0; i < columnCount(); i++) {
        QString header = headerData(i, Qt::Horizontal).toString();
        if (header == "duration") {
            m_formatters[i] = std::move(std::unique_ptr<Formatter>(new DurationFormatter));
        } else if (header == "bpm") {
            m_formatters[i] = std::move(std::unique_ptr<Formatter>(new BpmFormatter));
        }
    }
}

QVariant LibraryTableModel::SqlTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    QVariant result = QSqlTableModel::data(index, role);

    const auto& formatter = m_formatters[index.column()];
    if (formatter) {
        return formatter->format(result);
    }
    return result;
}
