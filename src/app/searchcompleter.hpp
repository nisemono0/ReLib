#pragma once

#include <QObject>
#include <QCompleter>

class SearchCompleter : public QCompleter {
    Q_OBJECT;
public:
    explicit SearchCompleter(QObject *parent = nullptr);
    ~SearchCompleter();

signals:

public slots:

};

