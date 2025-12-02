#pragma once

#include <QPlainTextEdit>
#include <QMenu>
#include <QAction>


class MangaTextEdit : public QPlainTextEdit {
    Q_OBJECT;
public:
    explicit MangaTextEdit(QWidget *parent = nullptr);
    ~MangaTextEdit();

private:
    QMenu *context_menu;
    QAction *copy_action;

    void initContextMenu();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void copy_action_triggered();

};

