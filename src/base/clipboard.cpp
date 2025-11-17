#include "base/clipboard.hpp"

#include <QApplication>
#include <QClipboard>


void Clipboard::setText(const QString &text) {
    QApplication::clipboard()->setText(text);
}

void Clipboard::setImage(const QImage &image) {
    QApplication::clipboard()->setImage(image);
}

