#include "app/imagescene.hpp"

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent) {
    this->image_worker = new ImageWorker();
    this->image_thread = new QThread(this);
    this->image_worker->moveToThread(this->image_thread);

}

ImageScene::~ImageScene() {
    if (this->image_thread->isRunning()) {
        this->image_thread->requestInterruption();
        this->image_thread->quit();
        this->image_thread->wait(1000);
        if (this->image_thread->isRunning()) {
            this->image_thread->terminate();
            this->image_thread->wait();
        }
        this->image_thread->deleteLater();
    }
    qDeleteAll(this->pixmap_items_list);
}

void ImageScene::clearPixmapItemsList() {
    qDeleteAll(this->pixmap_items_list);
    this->pixmap_items_list.clear();
}

