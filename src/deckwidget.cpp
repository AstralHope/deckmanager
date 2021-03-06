#include "deckwidget.h"
#include "draghelper.h"
#include "signaltower.h"
#include <QDebug>
#include <QApplication>

DeckWidget::DeckWidget(QWidget *parent, int _row, int _column, QSharedPointer<Type::DeckI> _deck)
    : QWidget(parent), deck(_deck), row(_row), column(_column), currentCardId(0),
      deckSize(-1), current(-1), overlapV(false)
{
    offset = QSize(3, 3);
    spacing = QSize(3, 3);
    cardSize = QSize(177 / 3.5 , 254 / 3.5);

    extFilter = [](quint32) {return true;};

    filter = [](quint32 id)
    {
        return !cardPool->getCard(id).isNull();
    };

    makeSnapShot = []() {};
    QSize min = cardSize + spacing;
    min.setWidth(min.width() * column + offset.width() * 2);
    min.setHeight(min.height() * row + offset.height() * 2);
    min = offset + min;
    setMouseTracking(true);
    setMinimumSize(min);

    setAcceptDrops(true);
}

void DeckWidget::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if(deckSize != deck->size())
    {
        emit sizeChanged(deck->size());
        deckSize = deck->size();
    }

    emit deckChanged(*deck);

    int cardPerRow = std::max(static_cast<int>(ceil(deckSize * 1.0 / row)), column);

    double cardHeight = (height() - offset.height() * 2.0) / row - spacing.height();
    double cardWidth = (width() - offset.width() * 2.0) / cardPerRow - spacing.width();
    double timesH = cardHeight / 254;
    double timesW = cardWidth / 177;
    double times = overlapV ? std::max(timesH, timesW) : timesH;
    cardSize = QSize(177 * times, 254 * times);

    int varWidth = (width() - offset.width()) * 1.0 - cardSize.width() - offset.width();
    int varHeight = (height() - offset.height() * 1.0 - cardSize.height()) - offset.height();
    int yoff = ((height() - offset.height()) * 1.0 - (cardSize.height() + spacing.height()) * row) / 2;
    int actualRow = static_cast<int>(ceil(deck->size() * 1.0 / cardPerRow));
    bool needOverlap = (actualRow * (cardSize.height() + spacing.height())) + offset.height() * 2 > height();
    auto it = deck->begin();
    for(int i = 0; i < row && it != deck->end(); i++)
    {
        for(int j = 0; j < cardPerRow && it != deck->end(); j++)
        {
            int x = offset.width() + floor(varWidth * j / (cardPerRow - 1));
            int y = 0;
            if(overlapV)
            {
                if(needOverlap)
                {
                    y = offset.height() + (actualRow <= 1 ? 0 : floor(varHeight * i) / (actualRow - 1));
                }
                else
                {
                    y = offset.height() + i * (cardSize.height() + spacing.height());
                }
            }
            else
            {
                y = yoff + offset.height() + i * (cardSize.height() + spacing.height());
            }
            it->setPos(QPoint(x, y));
            painter.drawPixmap(x, y, cardSize.width(),
                               cardSize.height(), *it->getPixmap().data());

            int lim = limitCards->getLimit(it->getId());
            if(lim < 3)
            {
                auto data = limitCards->getPixmap(lim);
                if(data)
                {
                    painter.drawPixmap(x, y, 16, 16, *data);
                }
            }
            if(current == i * cardPerRow + j)
            {
                QPen pen = painter.pen(), newPen = painter.pen();
                newPen.setWidth(2);
                newPen.setColor(QColor("darkgray"));
                painter.setPen(newPen);
                painter.drawRect(QRect(QPoint(x - 1,  y - 1), cardSize + QSize(1, 1)));
                painter.setPen(pen);
            }
            it++;
        }
    }
    painter.drawRect(QRect(QPoint(0, 0), QWidget::size() - QSize(1, 1)));
}

int DeckWidget::itemAt(const QPoint &pos)
{
    int i = deck->size();
    for(auto it = deck->end(); it != deck->begin() && i > 0;)
    {
        --it;
        --i;
        QPoint cardPos = it->getPos();
        if(pos.x() > cardPos.x() && pos.y() > cardPos.y())
        {
            if(pos.x() < cardPos.x() + cardSize.width() &&
                    pos.y() < cardPos.y() + cardSize.height())
            {
                return i;
            }
        }
    }
    return -1;
}

void DeckWidget::checkLeave()
{

    int i = itemAt(mapFromGlobal(QCursor::pos()));
    if(i != current)
    {
        current = i;
        update();
    }
}

int DeckWidget::posIndex(const QPoint &pos)
{
    int i = deck->size();
    bool found = false;
    for(auto it = deck->end(); it != deck->begin() && i > 0;)
    {
        --it;
        --i;
        QPoint cardPos = it->getPos();
        if(pos.x() > cardPos.x() && pos.y() > cardPos.y())
        {
            if(pos.y() < cardPos.y() + cardSize.height() + spacing.height())
            {
                found = true;
                break;
            }
        }
    }

    if(!found)
    {
        return -1;
    }
    else
    {
        int center = deck->at(i).getPos().x() + cardSize.width() / 2;
        if(pos.x() > center && i < deck->size())
        {
            return i + 1;
        }
        else
        {
            return i;
        }
    }
}

void DeckWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        startPos = event->pos();
    }
    else if(event->buttons() & Qt::RightButton)
    {
        deleteCard(event->pos());
    }
    else if(event->buttons() & Qt::MiddleButton)
    {
        int index = itemAt(event->pos());
        if(index >= 0)
        {
            quint32 id = deck->at(index).getId();
            tower->cardDetails(id);
        }
    }
    QWidget::mousePressEvent(event);
}

void DeckWidget::mouseMoveEvent(QMouseEvent *event)
{
    int index = itemAt(event->pos());
    if(index != -1)
    {
        quint32 id = deck->at(index).getId();
        if(currentCardId != id)
        {
            currentCardId = id;
            tower->changeCurrentId(id);
        }
    }

    if(event->buttons() & Qt::LeftButton)
    {
        int dist = (event->pos() - startPos).manhattanLength();
        if(dist >= QApplication::startDragDistance() && index != -1)
        {
            current = -1;
            startDrag(index);
        }
    }
    else if(current != index)
    {
        current = index;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void DeckWidget::startDrag(int index)
{
    auto *mimedata = new QMimeData;
    CardItem card(deck->at(index));
    mimedata->setText(QString::number(card.getId()));
    auto *drag = new QDrag(this);
    drag->setMimeData(mimedata);
    if(card.getPixmap())
    {
        drag->setPixmap(card.getPixmap()->scaled(cardSize));
        drag->setHotSpot(QPoint(drag->pixmap().width() / 2,
                                drag->pixmap().height() / 2));
    }
    makeSnapShot();
    bool copy = true;
    dragHelper.atomic = true;
    if((QApplication::keyboardModifiers() & Qt::ControlModifier) == 0)
    {
        deck->removeAt(index);
        copy = false;
    }
    emit sizeChanged(deck->size());
    emit deckChanged(*deck);
    update();
    dragHelper.moved = false;
    drag->exec(Qt::MoveAction);
    if(!dragHelper.moved && !copy)
    {
        deck->append(card);
    }
    dragHelper.atomic = false;
    update();
}
void DeckWidget::setDeck(const QSharedPointer<Type::DeckI> value)
{
    deck = value;
}


void DeckWidget::dragEnterEvent(QDragEnterEvent *event)
{
    auto src = event->source();

    if(src)
    {
        quint32 id = event->mimeData()->text().toUInt();
        if(filter(id) && extFilter(id))
        {
          event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

void DeckWidget::dragMoveEvent(QDragMoveEvent *event)
{
    auto src = event->source();
    if(src)
    {
        event->accept();
    }
}

void DeckWidget::dropEvent(QDropEvent *event)
{
    auto src = event->source();
    if(src)
    {
        quint32 id = event->mimeData()->text().toUInt();
        if(filter(id) && extFilter(id))
        {
            if(!dragHelper.atomic)
            {
                makeSnapShot();
            }
            int index = posIndex(event->pos());
            if(index == -1)
            {
                addCard(id);
            }
            else
            {
                insertCard(index, id);
            }
            dragHelper.moved = true;
            update();
        }
        event->accept();
    }
}

void DeckWidget::deleteCard(QPoint _pos)
{
    int index = itemAt(_pos);
    makeSnapShot();
    deck->removeAt(index);
    update();
}

int DeckWidget::countCard(quint32 id)
{
    return call_with_def([&](Card &card) {
        int sum = 0;
        foreach(auto &item, *deck)
        {
            call_with_ref([&](Card &card2) {
                if(id == card2.id)
                {
                    sum += 1;
                }
                else if(id == card2.alias)
                {
                    sum += 1;
                }
                else if(card.alias == card2.id)
                {
                    sum += 1;
                }
                else if(card.alias == card2.alias && card.alias != 0)
                {
                    sum += 1;
                }
            }, cardPool->getCard(item.getId()));
        }
        return sum;
    }, 0, cardPool->getCard(id));
}

void DeckWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        if(itemAt(mapFromGlobal(QCursor::pos())) >= 0)
        {
            tower->IdClick(currentCardId);
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

DeckWidget::~DeckWidget()
{

}

