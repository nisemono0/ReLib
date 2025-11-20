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

    // Add cover image or list of images to the scene
    void addCoverImage(QGraphicsPixmapItem *cover_image);
    void setImageList(const QMap<int, QGraphicsPixmapItem*> &pixmap_items);

    // Show next/prev image
    void showNextImage();
    void showPreviousImage();

    // Copy the current displayed image to clipboard
    void copyCurrentImage();

    // Jump to a loaded image number
    void jumpToImage(int image_number);

    // Clears scene and deletes points
    void clearScene();

    // Return true if only 1 image is loaded
    bool isCoverImage();

    // Return current/total image number
    int getCurrentImageNumber();
    int getTotalImagesNumber();

private:
    QMap<int, QGraphicsPixmapItem*> image_list;

    int current_image;
    int total_images;

    void sendCurrentSceneInfo();

signals:
    void send_ImageScene_info(int current_image, int total_images, int image_width, int image_height);

public slots:

};

