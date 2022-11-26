#include "headerview.h"

#include <QDebug>
#include <QtGui/QCursor>
#include <cmath>

HeaderView::HeaderView(QQuickItem* parent)
        : QQuickItem(parent) {
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
}

void HeaderView::hoverMoveEvent(QHoverEvent* event) {
    if (atColumnEdge(event->position().x())) {
        setCursor(Qt::SplitHCursor);
    } else {
        unsetCursor();
    }
}

QQuickItem* HeaderView::atColumnEdge(qreal xpos) const {
    for (auto item : childItems()) {
        if (xpos > item->x() + item->width() + m_columnSpacing * 0.5 - 6.0 &&
                xpos < item->x() + item->width() + m_columnSpacing * 0.5 + 6.0) {
            return item;
        }
    }
    return nullptr;
}

void HeaderView::hoverEnterEvent(QHoverEvent* event) {
    if (atColumnEdge(event->position().x())) {
        setCursor(Qt::SplitHCursor);
    }
}

void HeaderView::hoverLeaveEvent(QHoverEvent* event) {
    unsetCursor();
}

void HeaderView::mouseMoveEvent(QMouseEvent* event) {
    if (!m_dragItem) {
        return;
    }
    int i = childItems().indexOf(m_dragItem);
    qreal newWidth = std::round(event->position().x() + m_dragDelta - childItems()[i]->x());
    qreal change = newWidth - childItems()[i]->width();
    while (change) {
        if (childItems()[i]->width()) {
            newWidth = std::max(20.0, std::round(childItems()[i]->width() + change));
            qreal changed = newWidth - childItems()[i]->width();
            childItems()[i]->setWidth(newWidth);
            change -= changed;
        }
        if (change) {
            if (i == 0) {
                change = 0.0;
            } else {
                i--;
            }
        }
    }
    i++;
    while (i < childItems().size()) {
        qreal w = childItems()[i - 1]->width();
        if (w) {
            w += m_columnSpacing;
        }
        childItems()[i]->setX(childItems()[i - 1]->x() + w);
        i++;
    }
    if (i != 0) {
        setWidth(childItems()[i - 1]->x() + childItems()[i - 1]->width());
    }
}

void HeaderView::mousePressEvent(QMouseEvent* event) {
    m_dragItem = atColumnEdge(event->position().x());
    if (m_dragItem) {
        m_dragDelta = m_dragItem->x() + m_dragItem->width() - event->position().x();
    }
}

void HeaderView::mouseReleaseEvent(QMouseEvent* event) {
    if (!m_dragItem) {
        auto item = childAt(event->position().x(), event->position().y());
        if (item) {
            int index = childItems().indexOf(item);
            Qt::SortOrder sortOrder = static_cast<Qt::SortOrder>(item->property("sortOrder").toInt());
            if (index == m_sortingColumn) {
                sortOrder = sortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
                item->setProperty("sortOrder", sortOrder);
            } else {
                childItems()[m_sortingColumn]->setProperty("isSorting", false);
                m_sortingColumn = index;
                childItems()[m_sortingColumn]->setProperty("isSorting", true);
            }
            emit sort(m_sortingColumn, sortOrder);
        }
    }
    unsetCursor();
}

void HeaderView::componentComplete() {
    if (m_model) {
        qreal x = 0.0;
        qreal w = 0.0;
        for (int i = 0; i < m_model->columnCount(); i++) {
            if (w)
                x += m_columnSpacing;
            QString label = m_model->headerData(i, Qt::Horizontal).toString();
            auto item = dynamic_cast<QQuickItem*>(m_delegate->createWithInitialProperties({{"sortOrder", Qt::AscendingOrder}, {"isSorting", false}, {"label", label}}, qmlContext(this)));
            w = (m_showColumns.empty() || m_showColumns.indexOf(label) != -1) ? 100.0 : 0.0;
            item->setX(x);
            item->setY(0.0);
            item->setSize({static_cast<qreal>(w), height()});
            item->setParentItem(this);
            x += w;
        }
        setWidth(x);
    }
    QQuickItem::componentComplete();
}

qreal HeaderView::columnWidth(int column) {
    if (column >= 0 && column < childItems().size()) {
        return childItems()[column]->width();
    }
    return 0;
}
