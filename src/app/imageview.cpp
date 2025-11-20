#include "app/imageview.hpp"

#include "base/clipboard.hpp"

#include <QProgressDialog>


ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    // Set scene
    this->image_scene = new ImageScene(this);
    this->setScene(this->image_scene);

    // Init context menu
    this->initContextMenu();

    // Init ImageWorker thread
    this->image_worker = new ImageWorker();
    this->image_thread = new QThread(this);
    this->image_worker->moveToThread(this->image_thread);

    // ImageWorker requests
    connect(this, &ImageView::request_getArchiveCover, this->image_worker, &ImageWorker::receive_getArchiveCover_request);
    connect(this, &ImageView::request_getArchiveImages, this->image_worker, &ImageWorker::receive_getArchiveImages_request);

    // ImageWorker responses
    connect(this->image_worker, &ImageWorker::send_ImageWorker_info, this, &ImageView::receive_ImageWorker_info);
    connect(this->image_worker, &ImageWorker::send_ImageWorker_data, this, &ImageView::receive_ImageWorker_data);
    connect(this->image_worker, &ImageWorker::send_ImageWorker_cover, this, &ImageView::receive_ImageWorker_cover);
    connect(this->image_worker, &ImageWorker::send_ImageWorker_progress, this, &ImageView::receive_ImageWorker_progress);

    // ImageWorker thread
    connect(this->image_worker, &ImageWorker::send_ImageWorker_info, this->image_thread, &QThread::quit);
    connect(this->image_worker, &ImageWorker::send_ImageWorker_data, this->image_thread, &QThread::quit);
    connect(this->image_worker, &ImageWorker::send_ImageWorker_cover, this->image_thread, &QThread::quit);

    // Context menu
    connect(this->load_images_action, &QAction::triggered, this, &ImageView::load_images_action_triggered);
    connect(this->show_info_action, &QAction::triggered, this, &ImageView::show_info_action_triggered);
    connect(this->copy_image_action, &QAction::triggered, this, &ImageView::copy_image_action_triggered);
    connect(this->jump_image_action, &QAction::triggered, this, &ImageView::jump_image_action_triggered);
    connect(this->scroll_item_action, &QAction::triggered, this, &ImageView::scroll_item_action_triggered);

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

    delete this->load_images_action;
    delete this->show_info_action;
    delete this->copy_image_action;
    delete this->jump_image_action;
    delete this->scroll_item_action;
    delete this->context_menu;
}

void ImageView::initContextMenu() {
    this->context_menu = new QMenu("Image menu",this);
    this->load_images_action = new QAction("Load all images", this);
    this->show_info_action = new QAction("Show info", this);
    this->copy_image_action = new QAction("Copy image", this);
    this->jump_image_action = new QAction("Jump to image", this);
    this->scroll_item_action = new QAction("Scroll to item", this);

    this->context_menu->addAction(this->load_images_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->show_info_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->copy_image_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->jump_image_action);
    this->context_menu->addAction(this->scroll_item_action);
}

void ImageView::contextMenuEvent(QContextMenuEvent *event) {
    if (!this->image_scene->items().isEmpty()) {
        this->context_menu->popup(event->globalPos());
    }
    QGraphicsView::contextMenuEvent(event);
}

void ImageView::receive_LibraryView_currentChanged_path(const QString &file_path) {
    if (!this->image_thread->isRunning()) {
        this->image_thread->start();
        emit request_getArchiveCover(file_path);
    }
}

void ImageView::receive_LibraryVew_loadCurrentItemImages_path(const QString &file_path) {
    if (!this->image_thread->isRunning()) {
        this->image_thread->start();
        emit request_getArchiveImages(file_path);
    }
}

void ImageView::receive_ImageWorker_info(const QString &info) {
    emit send_ImageView_status(info);
}

void ImageView::receive_ImageWorker_progress(int progress) {
    if (this->imageview_progress_dialog) {
        this->imageview_progress_dialog->setValue(progress);
    } else {
        this->imageview_progress_dialog = new QProgressDialog(QStringLiteral("Extracting images"), QStringLiteral("Cancel"), 0, 100, this, Qt::FramelessWindowHint);
        this->imageview_progress_dialog->setCursor(Qt::WaitCursor);
        this->imageview_progress_dialog->setAttribute(Qt::WA_DeleteOnClose);
        this->imageview_progress_dialog->setFixedSize(this->imageview_progress_dialog->size());
        this->imageview_progress_dialog->show();
        this->imageview_progress_dialog->activateWindow();
        this->imageview_progress_dialog->setValue(progress);
        // Progress dialog cancel
        connect(this->imageview_progress_dialog, &QProgressDialog::canceled, this, &ImageView::imageview_progress_dialog_canceled);
    }
}

void ImageView::receive_ImageWorker_data(const QMap<int, QGraphicsPixmapItem*> &data, int total_images) {
    qDebug() << data;
}

void ImageView::receive_ImageWorker_cover(QGraphicsPixmapItem* pixmap_item, int total_images) {
    this->image_scene->addItem(pixmap_item);
}

void ImageView::load_images_action_triggered() {
    if (!this->image_thread->isRunning()) {
        emit request_LibraryView_loadCurrentItemImages();
    }
}

void ImageView::show_info_action_triggered() {
    emit request_LibraryView_showMangaInfoDialog();
}

void ImageView::copy_image_action_triggered() {

}

void ImageView::jump_image_action_triggered() {

}

void ImageView::scroll_item_action_triggered() {

}

void ImageView::imageview_progress_dialog_canceled() {
    if (this->image_thread->isRunning()) {
        this->image_thread->requestInterruption();
    }
}

