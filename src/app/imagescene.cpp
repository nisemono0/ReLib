#include "app/imagescene.hpp"

#include "base/clipboard.hpp"
#include "base/settings.hpp"

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent) {
    this->image_item = this->addPixmap(QPixmap());
    this->image_item->setVisible(true);

    this->current_image = -1;
    this->total_images = -1;
}

ImageScene::~ImageScene() {
    delete this->image_item;
}

void ImageScene::addCoverImage(const QPixmap &cover_pixmap) {
    this->clearScene();

    this->image_list[0] = cover_pixmap;

    this->image_item->setPixmap(cover_pixmap); // TODO: image_item->setPixmap(scalePixmap(cover_pixmap))

    this->image_item->show();

    this->total_images = 0;
    this->current_image = 0;
}

void ImageScene::setImageList(const QMap<int, QPixmap> &pixmap_map) {
    // Clear the scene
    this->clearScene();

    // Store pixmap_map
    this->image_list = pixmap_map;

    // Display the first image
    this->image_item->setPixmap(this->image_list[0]); // TODO: set scaled image

    // Set current and total image count
    this->current_image = 0;
    this->total_images = this->image_list.count();
}

void ImageScene::showNextImage() {
    if (this->current_image < this->total_images - 1) {
        this->current_image += 1;
        this->image_item->setPixmap(this->image_list[this->current_image]); // TODO: scale here
        this->sendCurrentSceneInfo();
    }
}

void ImageScene::showPreviousImage() {
    if (this->current_image > 0) {
        this->current_image -= 1;
        this->image_item->setPixmap(this->image_list[this->current_image]); // TODO scale here
        this->sendCurrentSceneInfo();

    }
}

void ImageScene::copyCurrentImage() {
    if (this->image_list.isEmpty()) {
        return;
    }
    Clipboard::setImage(this->image_list[this->current_image]);
}

void ImageScene::jumpToImage(int image_number) {
    if (image_number > this->total_images || image_number < 0) {
        return;
    }

    this->current_image = image_number;

    this->image_item->setPixmap(this->image_list[this->current_image]); // TODO scale here

    this->sendCurrentSceneInfo();

}

void ImageScene::clearScene() {
    this->image_list.clear();
    this->image_item->setPixmap(QPixmap());
    this->current_image = -1;
    this->total_images = -1;
}

bool ImageScene::isCoverImage() {
    if (this->image_list.count() <= 1) {
        return true;
    }
    return false;
}

QRectF ImageScene::getCurrentImageBoundingRect() {
    if (this->image_list.isEmpty()) {
        return QRectF();
    }

    return this->image_item->boundingRect();
}

int ImageScene::getCurrentImageNumber() {
    return this->current_image;
}

int ImageScene::getTotalImagesNumber() {
    return this->total_images;
}

void ImageScene::scaleCurrentImage() {
    // TODO scale here
    emit request_fitImage();
}

void ImageScene::sendCurrentSceneInfo() {
    emit send_ImageScene_info(
            this->current_image + 1,
            this->total_images,
            // Use original image size instead of the loaded item
            // since the item could be scaled
            this->image_list[this->current_image].width(),
            this->image_list[this->current_image].height()
            );
}

