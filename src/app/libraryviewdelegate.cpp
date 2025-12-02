#include "app/libraryviewdelegate.hpp"
#include "app/librarymodel.hpp"

#include <QToolTip>
#include <QStringBuilder>


LibraryViewDelegate::LibraryViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {

}

LibraryViewDelegate::~LibraryViewDelegate() {

}

QString LibraryViewDelegate::getToolTipText(const QModelIndex &index) {
    QString tooltip_text = QString();

    QString title = index.data(LibraryModel::Title).toString();
    QStringList artist = index.data(LibraryModel::Artist).toStringList();
    QStringList parody = index.data(LibraryModel::Parody).toStringList();
    QStringList circle = index.data(LibraryModel::Circle).toStringList();
    QStringList magazine = index.data(LibraryModel::Magazine).toStringList();
    QStringList event = index.data(LibraryModel::Event).toStringList();
    QStringList publisher = index.data(LibraryModel::Publisher).toStringList();
    QStringList tags = index.data(LibraryModel::Tags).toStringList();

    if (!title.isNull()) {
        tooltip_text.append(QStringLiteral("Title: %1\n").arg(title));
    }
    if (!artist.isEmpty()) {
        tooltip_text.append(QStringLiteral("Artist: %1\n").arg(artist.join("; ")));
    }
    if (!parody.isEmpty()) {
        tooltip_text.append(QStringLiteral("Parody: %1\n").arg(parody.join("; ")));
    }
    if (!circle.isEmpty()) {
        tooltip_text.append(QStringLiteral("Circle: %1\n").arg(circle.join("; ")));
    }
    if (!magazine.isEmpty()) {
        tooltip_text.append(QStringLiteral("Magazine: %1\n").arg(magazine.join("; ")));
    }
    if (!event.isEmpty()) {
        tooltip_text.append(QStringLiteral("Event: %1\n").arg(event.join("; ")));
    }
    if (!publisher.isEmpty()) {
        tooltip_text.append(QStringLiteral("Publisher: %1\n").arg(publisher.join("; ")));
    }
    if (!tags.isEmpty()) {
        tooltip_text.append(QStringLiteral("Tags: %1\n").arg(tags.join("; ")));
    }

    if (!tooltip_text.isEmpty()) {
        tooltip_text.chop(1); // Remove the last \n
    }

    return tooltip_text;
}

bool LibraryViewDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::ToolTip) {
        if (index.isValid()) {
            QToolTip::showText(
                    event->globalPos(),
                    this->getToolTipText(index),
                    view->viewport(),
                    view->visualRect(index)
                    );
            return true;
        }
    }
    return QStyledItemDelegate::helpEvent(event, view, option, index);
}

