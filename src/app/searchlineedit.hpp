#pragma once

#include "base/manga.hpp"

#include "app/searchcompleter.hpp"

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QList>


class SearchLineEdit : public QLineEdit {
    Q_OBJECT;
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit();

private:
    SearchCompleter *search_completer;

signals:
    void request_updateCompletionMode(SearchCompleter::CompleterRole role, const QString &text);

public slots:
    void receive_setCompleterData_request(const QList<Manga> &data);

private slots:
    void searchLineEdit_textChanged(const QString &text);
};

