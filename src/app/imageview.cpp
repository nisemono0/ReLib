#include "app/imageview.hpp"


ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    this->image_scene = new ImageScene(this);
    this->setScene(this->image_scene);
}

ImageView::~ImageView() {
    delete this->image_scene;
}

