#pragma once

#include "base/manga.hpp"

#include "app/searchcompleter.hpp"

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QList>
#include <QKeyEvent>


class SearchLineEdit : public QLineEdit {
    Q_OBJECT;
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit();

private:
    SearchCompleter *search_completer;
    // Inserts bracket in the text edit and moves the
    // cursor back once; used to insert matching brackets
    void insertMatchingBracket(const QString &bracket);
    // Remove the matching bracket if it exists
    // Example: pressing backspace here {|} would remove both {}
    // Return true if successfull
    bool removeMatchingBracket();

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void request_updateCompletionMode(SearchCompleter::CompleterRole role, const QString &text);

public slots:
    void receive_setCompleterData_request(const QList<Manga> &data);

private slots:
    void searchLineEdit_textChanged(const QString &text);
};

