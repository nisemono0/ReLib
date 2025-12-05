#pragma once

#include "base/manga.hpp"

#include "app/searchcompleter.hpp"

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QList>
#include <QKeyEvent>
#include <QRegularExpression>


class SearchLineEdit : public QLineEdit {
    Q_OBJECT;
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    ~SearchLineEdit();

    void setCompleter(SearchCompleter *search_completer);
    SearchCompleter* completer() const;

private:
    SearchCompleter *search_completer = nullptr;

    QRegularExpression completer_regex;

    // Inserts bracket in the text edit and moves the
    // cursor back once; used to insert matching brackets
    void insertMatchingBracket(const QString &bracket);
    // Remove the matching bracket if it exists
    // Example: pressing backspace here {|} would remove both {}
    // Return true if successfull
    bool removeMatchingBracket();
    // Checks if current cursor position is inside the {} braces
    // of a matched namespace
    bool isCursorInsideBrace(const QString &matched_text, const QString &matched_namespace);
    // This function checks if the next character is a closed bracket
    // and jumps over it if the input is the same as bracket, otherwise
    // it inserts bracket
    void jumpOrInsertClosedBracket(const QString &bracket);
    // Returns the ComplerRole of the matched tag namespace; For example
    // for artist:{} this will return SearchCompleter::Artist role
    SearchCompleter::CompleterRole getCompleterRoleFromNamespace(const QString &matched_namespace);

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void request_updateCompletionMode(SearchCompleter::CompleterRole role, const QString &prefix);

public slots:
    void receive_setCompleterData_request(const QList<Manga> &data);

private slots:
    void searchLineEdit_textEdited(const QString &text);
    void insertCompleterText(const QString &completer_text);

};

