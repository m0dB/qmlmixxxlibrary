#pragma once

#include <QAbstractItemModel>
#include <QQmlComponent>
#include <QtQuick/QQuickItem>

class HeaderView : public QQuickItem {
  private:
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QQmlComponent* delegate MEMBER m_delegate)
    Q_PROPERTY(QAbstractItemModel* model MEMBER m_model)
    Q_PROPERTY(qreal columnSpacing MEMBER m_columnSpacing)
    Q_PROPERTY(qreal tableViewColumnSpacing MEMBER m_tableViewColumnSpacing)
    Q_PROPERTY(QStringList showColumns MEMBER m_showColumns)

    QQmlComponent* m_delegate{};
    QAbstractItemModel* m_model{};

    qreal m_columnSpacing{};
    qreal m_tableViewColumnSpacing{};
    int m_sortingColumn{};
    QQuickItem* m_dragItem{};
    qreal m_dragDelta{};
    QStringList m_showColumns;

    void hoverEnterEvent(QHoverEvent* event) override;
    void hoverLeaveEvent(QHoverEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    QQuickItem* atColumnEdge(qreal xpos) const;

  public:
    HeaderView(QQuickItem* parent = nullptr);
    void componentComplete() override;
    Q_INVOKABLE qreal columnWidth(int column);
  signals:
    void sort(int column, Qt::SortOrder sortOrder);
};
