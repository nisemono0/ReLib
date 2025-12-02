#pragma once

#include <QWidget>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QEvent>
#include <QHelpEvent>


class LibraryViewDelegate : public QStyledItemDelegate {
    Q_OBJECT;
public:
    explicit LibraryViewDelegate(QObject *parent = nullptr);
    ~LibraryViewDelegate();

private:
    QString getToolTipText(const QModelIndex &index);

protected:
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

};

