#include "gui/mangainfodialog.hpp"

#include <QString>


MangaInfoDialog::MangaInfoDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MangaInfoDialog) {
    this->ui->setupUi(this);

    connect(this->ui->pushButtonCloseDialog, &QPushButton::clicked, this, &MangaInfoDialog::hide);
}


MangaInfoDialog::~MangaInfoDialog() {
    delete this->ui;
}

void MangaInfoDialog::updateDialogInfo(const Manga manga) {
    this->ui->fileHashPlainTextEdit->setPlainText(manga.file_hash);
    this->ui->filePathPlainTextEdit->setPlainText(manga.file_path);
    this->ui->fileBasenamePlainTextEdit->setPlainText(manga.file_basename);
    this->ui->titlePlainTextEdit->setPlainText(manga.title);
    this->ui->artistPlainTextEdit->setPlainText(manga.artist.join("; "));
    this->ui->parodyPlainTextEdit->setPlainText(manga.parody.join("; "));
    this->ui->circlePlainTextEdit->setPlainText(manga.circle.join("; "));
    this->ui->magazinePlainTextEdit->setPlainText(manga.magazine.join("; "));
    this->ui->eventPlainTextEdit->setPlainText(manga.event.join("; "));
    this->ui->publisherPlainTextEdit->setPlainText(manga.publisher.join("; "));
    this->ui->tagsPlainTextEdit->setPlainText(manga.tags.join("; "));
}

void MangaInfoDialog::receive_showMangaInfoDialog_request(const Manga manga) {
    this->updateDialogInfo(manga);
    if (this->isVisible()) {
        this->activateWindow();
    } else {
        this->show();
    }
}

