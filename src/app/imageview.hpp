#pragma once

#include "app/imagescene.hpp"

#include <QGraphicsView>
#include <QWidget>

#include <QEvent>


class ImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView() override;

private:
    ImageScene *image_scene;
    // TODO
    // Implement functions to load all images, next, prev, zoom etc.

signals:
    void send_ImageView_status(const QString &status);

public slots:
};

