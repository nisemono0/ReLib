#include "app/imageview.hpp"

#include "base/settings.hpp"

#include <QScrollBar>


ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    // This makes mouse events respond to movement
    this->setMouseTracking(true);
    // Load custom next/prev cursor image
    this->cursor_next = QCursor(QPixmap(":mouse/mouse-image-next"));
    this->cursor_previous = QCursor(QPixmap(":mouse/mouse-image-prev"));

    // Set scene
    this->image_scene = new ImageScene(this);
    this->image_scene->setViewWindowSize(QWidget::window()->size());
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

    // ImageScene current image info
    connect(this->image_scene, &ImageScene::send_ImageScene_info, this, &ImageView::receive_ImageScene_info);

    // ImageScene scale and fit the ImageView
    connect(this->image_scene, &ImageScene::request_fitImage, this, &ImageView::receive_fitImage_request);

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
    this->context_menu = new QMenu(QStringLiteral("Image menu"),this);
    this->load_images_action = new QAction(QStringLiteral("Load all images"), this);
    this->show_info_action = new QAction(QStringLiteral("Show info"), this);
    this->copy_image_action = new QAction(QStringLiteral("Copy image"), this);
    this->jump_image_action = new QAction(QStringLiteral("Jump to image"), this);
    this->scroll_item_action = new QAction(QStringLiteral("Scroll to item"), this);

    this->context_menu->addAction(this->load_images_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->show_info_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->copy_image_action);
    this->context_menu->addSeparator();

    this->context_menu->addAction(this->jump_image_action);
    this->context_menu->addAction(this->scroll_item_action);
}

void ImageView::updateImageViewStatus(int current_image, int total_images, int image_width, int image_height, bool is_cover) {
    if (is_cover) {
        emit send_ImageView_status(QStringLiteral("Cover: [%1/%2] (%3x%4)").arg(
                    QString::number(current_image),
                    QString::number(total_images),
                    QString::number(image_width),
                    QString::number(image_height))
                );
    } else {
        emit send_ImageView_status(QStringLiteral("Image: [%1/%2] (%3x%4)").arg(
                    QString::number(current_image),
                    QString::number(total_images),
                    QString::number(image_width),
                    QString::number(image_height))
                );
    }
}

void ImageView::updateImageViewStatus(const QString &status) {
    emit send_ImageView_status(status);
}

void ImageView::scaleImage() {
    // TODO
}

// TODO: remake this with pixmap instead of QGraphicsPixmapItem
void ImageView::fitImage() {
    this->setSceneRect(this->image_scene->getCurrentImageBoundingRect());
    switch (Settings::image_view_option) {
        case ImageOptions::FitInView:
            {
                this->fitInView(this->image_scene->getCurrentImageBoundingRect(), Qt::KeepAspectRatio);
                break;
            }
        case ImageOptions::FitToWidth:
            {
                int scroll_h = this->horizontalScrollBar()->value();
                int scroll_y = this->horizontalScrollBar()->value();
                this->fitInView(this->image_scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
                if (this->sceneRect().width() >= this->mapToScene(this->viewport()->rect()).boundingRect().width()) {
                    this->fitInView(this->image_scene->getCurrentImageBoundingRect(), Qt::KeepAspectRatio);
                }
                this->horizontalScrollBar()->setValue(scroll_h);
                this->verticalScrollBar()->setValue(scroll_y);
                break;
            }
        case ImageOptions::FreeView:
            {
                this->setTransformationAnchor(QGraphicsView::NoAnchor);
                this->setResizeAnchor(QGraphicsView::NoAnchor);
                this->setDragMode(QGraphicsView::ScrollHandDrag);
                break;
            }
    };
}

void ImageView::contextMenuEvent(QContextMenuEvent *event) {
    QGraphicsView::contextMenuEvent(event);
    if (!this->image_scene->items().isEmpty()) {
        this->context_menu->popup(event->globalPos());
    }
}

void ImageView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);
    if (this->image_scene->isCoverImage()) {
        this->setCursor(Qt::ArrowCursor);
    } else {
        int mouse_x_pos = event->position().x();
        int half_point = this->size().width() / 2;
        if (mouse_x_pos > half_point) {
            this->setCursor(this->cursor_next);
        } else {
            this->setCursor(this->cursor_previous);
        }
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    this->setDragMode(QGraphicsView::NoDrag);
    if (event->button() == Qt::LeftButton) {
        int mouse_x_pos = event->position().x();
        int half_point = this->size().width() / 2;
        if (mouse_x_pos > half_point) {
            this->image_scene->showNextImage();
        } else {
            this->image_scene->showPreviousImage();
        }
    }
}

void ImageView::receive_LibraryView_currentChanged_path(const QString &file_path) {
    this->current_item_path = file_path;
    if (!this->image_thread->isRunning()) {
        this->image_thread->start();
        emit request_getArchiveCover(this->current_item_path);
    }
}

void ImageView::receive_LibraryVew_load_images_path(const QString &file_path) {
    if (!this->image_thread->isRunning()) {
        this->image_thread->start();
        emit request_getArchiveImages(file_path);
    }
}

void ImageView::receive_ImageWorker_info(const QString &info) {
    this->updateImageViewStatus(info);
    this->image_scene->clearScene();
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

void ImageView::receive_ImageWorker_data(const QMap<int, QPixmap> &data, int total_images) {
    this->image_scene->setImageList(data);
}

void ImageView::receive_ImageWorker_cover(const QPixmap &pixmap, int total_images) {
    this->image_scene->addCoverImage(pixmap);
    this->updateImageViewStatus(1, total_images, pixmap.width(), pixmap.height(), true);
}

void ImageView::receive_clearImageView_request() {
    if (this->image_thread->isRunning()) {
        this->image_thread->requestInterruption();
    }
    this->image_scene->clearScene();
    this->updateImageViewStatus("");
}

void ImageView::load_images_action_triggered() {
    if (!this->image_thread->isRunning()) {
        this->image_thread->start();
        emit request_getArchiveImages(this->current_item_path);
    }
}

void ImageView::show_info_action_triggered() {
    emit request_LibraryView_showMangaInfoDialog();
}

void ImageView::copy_image_action_triggered() {
    this->image_scene->copyCurrentImage();
}

void ImageView::jump_image_action_triggered() {
    if (this->jump_image_dialog) {
        this->jump_image_dialog->activateWindow();
    } else {
        if (this->image_scene->isCoverImage()) {
            return;
        }
        int current_image_number = this->image_scene->getCurrentImageNumber();
        int total_image_number = this->image_scene->getTotalImagesNumber();

        this->jump_image_dialog = new QInputDialog(this);
        this->jump_image_dialog->setLabelText(QStringLiteral("Jump to image [%1/%2]").arg(
                    QStringLiteral("1"),
                    QString::number(total_image_number)
                    ));
        this->jump_image_dialog->setAttribute(Qt::WA_DeleteOnClose);
        this->jump_image_dialog->setInputMode(QInputDialog::IntInput);
        this->jump_image_dialog->setFixedSize(this->jump_image_dialog->size());
        this->jump_image_dialog->setIntRange(1, total_image_number);
        this->jump_image_dialog->setIntValue(current_image_number + 1);
        this->jump_image_dialog->setIntStep(1);
        this->jump_image_dialog->show();
        this->jump_image_dialog->activateWindow();
        // Input dialog accept/cancel
        connect(this->jump_image_dialog, &QInputDialog::intValueSelected, this, [this](int value){
                this->image_scene->jumpToImage(value - 1);
                });
    }
}

void ImageView::scroll_item_action_triggered() {
    emit request_LibraryView_scrollToCurrentItem();
}

void ImageView::receive_ImageScene_info(int current_image, int total_images, int image_width, int image_height) {
    this->updateImageViewStatus(current_image, total_images, image_width, image_height);
}

void ImageView::imageview_progress_dialog_canceled() {
    if (this->image_thread->isRunning()) {
        this->image_thread->requestInterruption();
    }
}

void ImageView::receive_scaleAndFitImage_request() {
    this->image_scene->scaleCurrentImage();
    this->fitImage();
}

void ImageView::receive_fitImage_request() {
    this->fitImage();
}

