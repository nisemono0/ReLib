#include "app/imageview.hpp"

#include <QProgressDialog>


ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    // Set scene
    this->image_scene = new ImageScene(this);
    this->setScene(this->image_scene);

    // Init ImageWorker thread
    this->image_worker = new ImageWorker();
    this->image_thread = new QThread(this);
    this->image_worker->moveToThread(this->image_thread);
}

ImageView::~ImageView() {
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
    delete this->image_scene;
}

void ImageView::receive_ImageWorker_info(const QString &info) {
    qDebug() << "ImageWorker_info " << info;
}

void ImageView::receive_ImageWorker_progress(int progress) {
    emit send_ImageView_progress(progress);
}

void ImageView::receive_ImageWorker_data(const QMap<int, QGraphicsPixmapItem*> &data, int total_images) {

}

void ImageView::receive_ImageWorker_cover(const QGraphicsPixmapItem* pixmap_item, int total_images) {

}

