#pragma once

#include "base/manga.hpp"

#include "app/searchcompleter.hpp"

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QList>
#include <QKeyEvent>
#include <QRegularExpression>


class SearchPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT;
public:
    explicit SearchPlainTextEdit(QWidget *parent = nullptr);
    ~SearchPlainTextEdit();

    void setCompleter(SearchCompleter *search_completer);
    SearchCompleter* completer() const;

    // Hides the completion window if visible
    void hideCompleter();
    // True if completer window is visible
    bool isCompleterVisible();
    // Selects the next completer entry
    void selectNextCompletion();
    // Selects the previous completer entry
    void selectPreviousCompletion();
    // Clears the completion
    void clearCompleter();

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
    // of a matched namespace; if current_cursor_pos is not given
    // the method cursorPosition(); will be used to get it
    bool isCursorInsideBrace(const QString &whole_text, const QString &namespace_text, const QString &tags_text);
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
    void returnPressed();

public slots:
    void receive_setCompleterData_request(const QList<Manga> &data);

private slots:
    void searchPlainTextEdit_textEdited();
    void searchPlainTextEdit_cursorPositionChanged();

    void receive_completerText(const QString &completer_text);

};

