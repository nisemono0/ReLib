#pragma once

#include <QObject>
#include <QWidget>
#include <QLineEdit>

class SearchLineEdit : public QLineEdit {
    Q_OBJECT;
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit();

signals:

public slots:
};

