#include "app/imagescene.hpp"

#include "base/clipboard.hpp"
#include "base/settings.hpp"

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent) {
    this->scene_item = this->addPixmap(QPixmap());
    this->scene_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene_item->setVisible(true);

    this->current_image = -1;
    this->total_images = -1;
}

ImageScene::~ImageScene() {
    delete this->scene_item;
}

void ImageScene::setViewWindowSize(const QSize &window_size) {
    this->view_window_size = window_size;
}

void ImageScene::addCoverImage(const QPixmap &cover_pixmap) {
    this->clearScene();

    this->image_list[0] = cover_pixmap;

    this->setCurrentDisplayImage(0);

    this->total_images = 0;
    this->current_image = 0;
}

void ImageScene::setImageList(const QMap<int, QPixmap> &pixmap_map) {
    // Clear the scene
    this->clearScene();

    // Store pixmap_map
    this->image_list = pixmap_map;

    // Display the first image
    this->setCurrentDisplayImage(0);

    // Set current and total image count
    this->current_image = 0;
    this->total_images = this->image_list.count();
}

void ImageScene::showNextImage() {
    if (this->current_image < this->total_images - 1) {
        // Increment current_image before displaying
        this->setCurrentDisplayImage(++this->current_image);
        this->sendCurrentSceneInfo();
    }
}

void ImageScene::showPreviousImage() {
    if (this->current_image > 0) {
        // Decrement current_image before displaying
        this->setCurrentDisplayImage(--this->current_image);
        this->sendCurrentSceneInfo();

    }
}

void ImageScene::copyCurrentImage() {
    if (this->isImageListEmpty()) {
        return;
    }
    Clipboard::setImage(this->image_list[this->current_image]);
}

void ImageScene::jumpToImage(int image_number) {
    if (image_number > this->total_images || image_number < 0) {
        return;
    }


    this->setCurrentDisplayImage(image_number);

    this->current_image = image_number;

    this->sendCurrentSceneInfo();

}

void ImageScene::clearScene() {
    this->image_list.clear();
    this->scene_item->setPixmap(QPixmap());
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
    if (this->isImageListEmpty()) {
        return QRectF();
    }

    return this->scene_item->boundingRect();
}

int ImageScene::getCurrentImageNumber() {
    return this->current_image;
}

int ImageScene::getTotalImagesNumber() {
    return this->total_images;
}

void ImageScene::scaleCurrentImage() {
    if (this->isImageListEmpty()) {
        return;
    }
    if (Settings::scale_image) {
        this->scene_item->setPixmap(this->image_list[this->current_image].scaled(
                    this->view_window_size * Settings::image_scale_value,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    ));
    } else {
        this->scene_item->setPixmap(this->image_list[this->current_image]);
    }
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

bool ImageScene::isImageListEmpty() {
    if (this->image_list.isEmpty()) {
        return true;
    }
    return false;
}

void ImageScene::setCurrentDisplayImage(int image_number) {
    if (this->isImageListEmpty()) {
        return;
    }

    if (this->image_list.contains(image_number)) {
        if (Settings::scale_image) {
            this->scene_item->setPixmap(this->image_list[image_number].scaled(
                        this->view_window_size * Settings::image_scale_value,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                        ));
        } else {
            this->scene_item->setPixmap(this->image_list[image_number]);
        }
        emit request_fitImage();
    }
}

