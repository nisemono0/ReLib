#pragma once

#include "app/searchcompleter.hpp"

#include <QObject>
#include <QWidget>
#include <QLineEdit>


class SearchLineEdit : public QLineEdit {
    Q_OBJECT;
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit();

private:
    SearchCompleter *search_completer;

signals:

public slots:
};

