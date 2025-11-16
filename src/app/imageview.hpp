#pragma once

#include <QGraphicsView>
#include <QWidget>


// TODO
// Implement loading images; Have a thread for
// reading images inside archives
class ImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView() override;

signals:

public slots:
};

