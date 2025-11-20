#include "base/clipboard.hpp"

#include <QApplication>
#include <QClipboard>


void Clipboard::setText(const QString &text) {
    QApplication::clipboard()->setText(text);
}

void Clipboard::setImage(const QImage &image) {
    QApplication::clipboard()->setImage(image);
}

void Clipboard::setImage(const QPixmap &pixmap) {
    QImage image = pixmap.toImage();
    QApplication::clipboard()->setImage(image);
}

void Clipboard::setImage(QGraphicsPixmapItem *pixmap_item) {
    QImage image = pixmap_item->pixmap().toImage();
    QApplication::clipboard()->setImage(image);
}

