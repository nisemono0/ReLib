#pragma once

#include "base/imageworker.hpp"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QThread>
#include <QMap>


class ImageScene : public QGraphicsScene {
    Q_OBJECT;
public:
    explicit ImageScene(QObject *parent = nullptr);
    ~ImageScene();

private:
    // Implement functions to load the pixmap items into the scene etc.
    QMap<int, QGraphicsPixmapItem*> pixmap_items_list;

    ImageWorker *image_worker;
    QThread *image_thread;

    void clearPixmapItemsList();

signals:

public slots:

};

