#include "app/imagescene.hpp"

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent) {

}

ImageScene::~ImageScene() {
    qDeleteAll(this->pixmap_items_list);
}

void ImageScene::clearPixmapItemsList() {
    qDeleteAll(this->pixmap_items_list);
    this->pixmap_items_list.clear();
}

