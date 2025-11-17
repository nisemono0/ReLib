#pragma once

#include <QString>
#include <QImage>


class Clipboard {
public:
    static void setText(const QString &text);
    static void setImage(const QImage &image);
};

