#include "app/imagescene.hpp"

#include "base/clipboard.hpp"

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent) {
    this->current_image = -1;
    this->total_images = -1;
}

ImageScene::~ImageScene() {
    qDeleteAll(this->image_list);
}

void ImageScene::addCoverImage(QGraphicsPixmapItem *cover_image) {
    // Clear scene before adding a new cover image
    this->clearScene();
    // Add cover image item and store its pointer for ease of access
    this->addItem(cover_image);
    this->image_list[0] = cover_image;
    // Show the added cover image
    this->image_list[0]->show();
    this->total_images = 0;
    this->current_image = 0;
}

void ImageScene::setImageList(const QMap<int, QGraphicsPixmapItem*> &pixmap_items) {
    // Clear the scene
    this->clearScene();
    // Add our pixmap items to the scene
    for (auto item : pixmap_items) {
        this->addItem(item);
    }
    // Store the item pointers for ease of access
    this->image_list = pixmap_items;
    // Display the first image
    this->image_list[0]->show();
    // Set current and total image count
    this->current_image = 0;
    this->total_images = this->image_list.count();
}

void ImageScene::showNextImage() {
    if (this->current_image < this->total_images - 1) {
        this->image_list[this->current_image]->hide();
        this->current_image += 1;
        this->image_list[this->current_image]->show();
        this->sendCurrentSceneInfo();
    }
}

void ImageScene::showPreviousImage() {
    if (this->current_image > 0) {
        this->image_list[this->current_image]->hide();
        this->current_image -= 1;
        this->image_list[this->current_image]->show();
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

    this->image_list[this->current_image]->hide();
    this->current_image = image_number;
    this->image_list[this->current_image]->show();

    this->sendCurrentSceneInfo();
}

void ImageScene::clearScene() {
    qDeleteAll(this->image_list);
    this->image_list.clear();
    this->clear();
    this->current_image = -1;
    this->total_images = -1;
}

bool ImageScene::isCoverImage() {
    if (this->image_list.count() <= 1) {
        return true;
    }
    return false;
}

int ImageScene::getCurrentImageNumber() {
    return this->current_image;
}

int ImageScene::getTotalImagesNumber() {
    return this->total_images;
}

void ImageScene::sendCurrentSceneInfo() {
    emit send_ImageScene_info(
            this->current_image + 1,
            this->total_images,
            this->image_list[this->current_image]->pixmap().width(),
            this->image_list[this->current_image]->pixmap().height()
            );
}

