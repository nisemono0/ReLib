#pragma once

#include <QObject>


class ImageWorker : public QObject {
    Q_OBJECT;
public:
    explicit ImageWorker(QObject *parent = nullptr);
    ~ImageWorker();
    // TODO
    // Implement functions to extract image files from archive
    // and load them into a QList<QPixmap>

private:

signals:

public slots:

};

