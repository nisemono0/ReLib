#pragma once

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
    void addCoverImage(const QPixmap &cover_pixmap);
    void setImageList(const QMap<int, QPixmap> &pixmap_map);

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

    // Return the current item's bounding rect
    QRectF getCurrentImageBoundingRect();

    // Return current/total image number
    int getCurrentImageNumber();
    int getTotalImagesNumber();

    // Scale the current image
    void scaleCurrentImage();

private:
    QMap<int, QPixmap> image_list;
    QGraphicsPixmapItem *image_item;

    int current_image;
    int total_images;

    void sendCurrentSceneInfo();

signals:
    void send_ImageScene_info(int current_image, int total_images, int image_width, int image_height);
    void request_fitImage();

public slots:

};

