#pragma once

#include <qqml.h>

#include <QAbstractTableModel>
#include <QSqlRecord>
#include <QThread>

class Worker : public QObject {
    Q_OBJECT
  public slots:
    void execute(const QString& queryString, const QString& signal);
  signals:
    void ready(const QList<QSqlRecord>& results);
    void readySort(const QList<QSqlRecord>& results);
    void totalRowCountReady(const QList<QSqlRecord>& results);
};

class Formatter {
  public:
    virtual ~Formatter() {
    }
    virtual QVariant format(const QVariant& variant) = 0;
};

class LibraryTableModel : public QAbstractTableModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int totalRowCount READ totalRowCount NOTIFY totalRowCountChanged)
    Q_PROPERTY(qreal targetContentY READ targetContentY WRITE setTargetContentY NOTIFY targetContentYChanged)
    Q_PROPERTY(qreal readyContentY READ readyContentY NOTIFY readyContentYChanged)

    QThread m_workerThread;

    std::vector<std::unique_ptr<Formatter>> m_formatters;

    QString m_sort;
    QStringList m_fieldNames;

    int m_totalRowCount{};
    QList<QSqlRecord> m_results;

    qreal m_targetContentY{};
    qreal m_readyContentY{};
    qreal m_pendingContentY{};

  public slots:
    void ready(const QList<QSqlRecord>& results);
    void readySort(const QList<QSqlRecord>& results);
    void totalRowCountReady(const QList<QSqlRecord>& records);
  signals:
    void execute(const QString& queryString, const QString& signal);

    void totalRowCountChanged(int);
    void targetContentYChanged(qreal value);
    void readyContentYChanged(qreal value);

  public:
    LibraryTableModel(QObject* parent = nullptr);
    ~LibraryTableModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void sort(int column, Qt::SortOrder sortOrder) override;

  private:
    int totalRowCount() {
        return m_totalRowCount;
    }

    qreal targetContentY() const {
        return m_targetContentY;
    }

    void setTargetContentY(qreal y);

    qreal readyContentY() const {
        return m_readyContentY;
    }

    void requestData(const QString& signal);
    void requestTotalRowCount();
};
