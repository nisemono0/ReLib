#pragma once

#include "ui/ui_mangainfodialog.h"

#include "base/manga.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
    class MangaInfoDialog;
}

class MangaInfoDialog : public QDialog {
    Q_OBJECT;
public:
    explicit MangaInfoDialog(QWidget *parent = nullptr);
    ~MangaInfoDialog() override;

private:
    Ui::MangaInfoDialog *ui;

    void updateDialogInfo(const Manga &manga);

public slots:
    void receive_showMangaInfoDialog_request(const Manga &manga);

};

