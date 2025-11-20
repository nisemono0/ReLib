#pragma once

#include <QString>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>


class Clipboard {
public:
    static void setText(const QString &text);
    static void setImage(const QImage &image);
    static void setImage(const QPixmap &pixmap);
    static void setImage(QGraphicsPixmapItem* pixmap_item);
};

