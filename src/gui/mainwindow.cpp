#include "gui/mainwindow.hpp"

#include "base/log.hpp"

#include "utils/str.hpp"

#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // Ui setup
    this->ui->setupUi(this);
    this->log_dialog = new LogDialog(this);
    this->manga_info_dialog = new MangaInfoDialog(this);

    this->updateUiLock();

    this->library_view_status = new QLabel(this);
    this->ui->statusBar->addWidget(this->library_view_status);
    this->library_view_status->setIndent(4);
    this->image_view_status = new QLabel(this);
    this->ui->statusBar->addPermanentWidget(this->image_view_status);
    this->setNoDatabaseStatus();

    this->action_scale_slider = new QWidgetAction(this->ui->menuSettings);
    this->slider_scale = new QSlider(this->ui->menuSettings);
    this->slider_scale->setFixedHeight(25);
    this->slider_scale->setOrientation(Qt::Horizontal);
    this->slider_scale->setTickPosition(QSlider::NoTicks);
    this->slider_scale->setMaximum(10);
    this->slider_scale->setMinimum(0);
    this->slider_scale->setSingleStep(0);
    this->slider_scale->setPageStep(0);
    this->slider_scale->setValue(0);
    this->action_scale_slider->setDefaultWidget(this->slider_scale);
    this->ui->menuSettings->insertAction(this->ui->menuSettingsView->menuAction(), this->action_scale_slider);
    this->ui->menuSettings->insertSeparator(this->ui->menuSettingsView->menuAction());

    this->actiongroup_view_mode = new QActionGroup(this->ui->menuSettingsView);
    this->actiongroup_view_mode->addAction(this->ui->actionFitInView);
    this->actiongroup_view_mode->addAction(this->ui->actionFitToWidth);
    this->actiongroup_view_mode->addAction(this->ui->actionFreeView);
    this->actiongroup_view_mode->setExclusive(true);

    // Thread setup
    this->zip_worker = new ZipWorker();
    this->zip_thread = new QThread(this);
    this->zip_worker->moveToThread(this->zip_thread);

    this->db_worker = new DBWorker();
    this->db_thread = new QThread(this);
    this->db_worker->moveToThread(this->db_thread);

    // Menubar:File
    connect(this->ui->actionAddFile, &QAction::triggered, this, &MainWindow::actionAddFile_triggered);
    connect(this->ui->actionAddDir, &QAction::triggered, this, &MainWindow::actionAddDir_triggered);
    connect(this->ui->actionExit, &QAction::triggered, this, &MainWindow::actionExit_triggered);

    // Menubar:Database
    connect(this->ui->actionCreateDatabase, &QAction::triggered, this, &MainWindow::actionCreateDatabase_triggered);
    connect(this->ui->actionLoadDatabase, &QAction::triggered, this, &MainWindow::actionLoadDatabase_triggered);
    connect(this->ui->actionUnloadDatabase, &QAction::triggered, this, &MainWindow::actionUnloadDatabase_triggered);
    connect(this->ui->actionCheckDatabaseHashes, &QAction::triggered, this, &MainWindow::actionCheckDatabaseHashes_triggered);
    connect(this->ui->actionCheckDatabasePaths, &QAction::triggered, this, &MainWindow::actionCheckDatabasePaths_triggered);

    // TODO: Menubar:Settings

    // Menubar:Info
    connect(this->ui->actionShowLogs, &QAction::triggered, this->log_dialog, &LogDialog::receive_showLogDialog_request);
    connect(this->ui->actionShowAbout, &QAction::triggered, this, &QApplication::aboutQt);

    // Buttons
    connect(this->ui->pushButtonSearch, &QPushButton::clicked, this, &MainWindow::pushButtonSearch_clicked);
    connect(this->ui->pushButtonRandom, &QPushButton::clicked, this, &MainWindow::pushButtonRandom_clicked);
    connect(this->ui->pushButtonRefresh, &QPushButton::clicked, this, &MainWindow::pushButtonRefresh_clicked);

    // Search
    connect(this->ui->lineEditSearch, &QLineEdit::returnPressed, this, &MainWindow::lineEditSearch_returnPressed);

    // Library view
    connect(this, &MainWindow::request_setMangaList, this->ui->libraryView, &LibraryView::receive_setMangaList_request);
    connect(this, &MainWindow::request_clearMangaList, this->ui->libraryView, &LibraryView::receive_clearMangaList_request);
    connect(this, &MainWindow::request_setSearchText, this->ui->libraryView, &LibraryView::receive_setSearchText_request);
    connect(this, &MainWindow::request_selectRandomManga, this->ui->libraryView, &LibraryView::receive_selectRandomManga_request);
    connect(this->ui->libraryView, &LibraryView::send_LibraryView_status, this->library_view_status, &QLabel::setText);
    connect(this->ui->libraryView, &LibraryView::send_LibraryView_deleteFromDatabase_request, this, &MainWindow::receive_LibraryView_deleteFromDatabse_request);
    connect(this->ui->libraryView, &LibraryView::request_showMangaInfoDialog, this->manga_info_dialog, &MangaInfoDialog::receive_showMangaInfoDialog_request);

    // Image view
    connect(this->ui->imageView, &ImageView::send_ImageView_status, this->image_view_status, &QLabel::setText);

    // Thread requests
    connect(this, &MainWindow::request_getFileJsonInfo, this->zip_worker, &ZipWorker::receive_getFileJsonInfo_request);
    connect(this, &MainWindow::request_getDirJsonInfo, this->zip_worker, &ZipWorker::receive_getDirJsonInfo_request);
    connect(this, &MainWindow::request_createDatabase, this->db_worker, &DBWorker::receive_createDatabase_request);
    connect(this, &MainWindow::request_loadDatabase, this->db_worker, &DBWorker::receive_loadDatabase_request);
    connect(this, &MainWindow::request_unloadDatabase, this->db_worker, &DBWorker::receive_unloadDatabase_request);
    connect(this, &MainWindow::request_insertInDatabase, this->db_worker, &DBWorker::receive_insertIntoDatabase_request);
    connect(this, &MainWindow::request_deleteFromDatabase, this->db_worker, &DBWorker::receive_deleteFromDatabase_request);
    connect(this, &MainWindow::request_getAllDatabaseData, this->db_worker, &DBWorker::receive_getAllDatabaseData_request);
    connect(this, &MainWindow::request_getPathHashDatabaseData, this->db_worker, &DBWorker::receive_getPathHashDatabaseData_request);
    connect(this, &MainWindow::request_checkDatabaseHashes, this->db_worker, &DBWorker::receive_checkDatabaseHashes_request);
    connect(this, &MainWindow::request_checkDatabaseFilepaths, this->db_worker, &DBWorker::receive_checkDatabaseFilepaths_request);

    // Thread info/data received
    connect(this->zip_worker, &ZipWorker::send_ZipWorker_info, this, &MainWindow::receive_ZipWorker_info);
    connect(this->zip_worker, &ZipWorker::send_ZipWorker_progress, this, &MainWindow::receive_ZipWorker_progress);
    connect(this->zip_worker, &ZipWorker::send_ZipWorker_data, this, &MainWindow::receive_ZipWorker_data);
    connect(this->db_worker, &DBWorker::send_DBWorker_info, this, &MainWindow::receive_DBWorker_info);
    connect(this->db_worker, &DBWorker::send_DBWorker_loadDatabase_status, this, &MainWindow::receive_DBWorker_loadDatabase_status);
    connect(this->db_worker, &DBWorker::send_DBWorker_unloadDatabase_status, this, &MainWindow::receive_DBWorker_unloadDatabase_status);
    connect(this->db_worker, &DBWorker::send_DBWorker_progress, this, &MainWindow::receive_DBWorker_progress);
    connect(this->db_worker, &DBWorker::send_DBWorker_data, this, &MainWindow::receive_DBWorker_data);
    connect(this->db_worker, &DBWorker::send_DBWorker_pathhash_data, this, &MainWindow::receive_DBWorker_pathhash_data);

    // Quit thread when result or info is received
    connect(this->zip_worker, &ZipWorker::send_ZipWorker_info, this->zip_thread, &QThread::quit);
    connect(this->zip_worker, &ZipWorker::send_ZipWorker_data, this->zip_thread, &QThread::quit);
    connect(this->db_worker, &DBWorker::send_DBWorker_info, this->db_thread, &QThread::quit);
    connect(this->db_worker, &DBWorker::send_DBWorker_unloadDatabase_status, this->db_thread, &QThread::quit);
    connect(this->db_worker, &DBWorker::send_DBWorker_data, this->db_thread, &QThread::quit);
    connect(this->db_worker, &DBWorker::send_DBWorker_pathhash_data, this->db_thread, &QThread::quit);
}

MainWindow::~MainWindow() {
    if (this->zip_thread->isRunning()) {
        this->zip_thread->requestInterruption();
        this->zip_thread->quit();
        this->zip_thread->wait(1000);
        if (this->zip_thread->isRunning()) {
            this->zip_thread->terminate();
            this->zip_thread->wait();
        }
        this->zip_thread->deleteLater();
    }
    if (this->db_thread->isRunning()) {
        this->db_thread->requestInterruption();
        this->db_thread->quit();
        this->db_thread->wait(1000);
        if (this->db_thread->isRunning()) {
            this->db_thread->terminate();
            this->db_thread->wait();
        }
        this->db_thread->deleteLater();
    }

    delete this->db_worker;
    delete this->db_thread;

    delete this->zip_worker;
    delete this->zip_thread;

    delete this->mainwindow_progress_dialog;
    delete this->slider_scale;
    delete this->action_scale_slider;

    delete this->library_view_status;
    delete this->image_view_status;

    delete this->log_dialog;
    delete this->manga_info_dialog;
    delete this->ui;
}

QString MainWindow::selectFile(const FileDialog::FileDialog DIALOG_TYPE) {
    QFileDialog file_dialog = QFileDialog(this, QStringLiteral("Select file"), QDir::homePath());

    switch (DIALOG_TYPE) {
        case FileDialog::SelectDB:
            file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
            file_dialog.setFileMode(QFileDialog::ExistingFile);
            file_dialog.setNameFilter(QStringLiteral("SQLite files (*.db *.sqlite)"));
            break;
        case FileDialog::SelectDBSave:
            file_dialog.setAcceptMode(QFileDialog::AcceptSave);
            file_dialog.setFileMode(QFileDialog::AnyFile);
            file_dialog.setNameFilter(QStringLiteral("SQLite files (*.db *.sqlite)"));
            file_dialog.setDefaultSuffix(QStringLiteral(".db"));
            break;
        case FileDialog::SelectZip:
            file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
            file_dialog.setFileMode(QFileDialog::ExistingFile);
            file_dialog.setNameFilters({
                    QStringLiteral("Zip files (*.zip)"),
                    QStringLiteral("Cbz files (*.cbz)")
                    });
            break;
        case FileDialog::SelectDir:
            file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
            file_dialog.setFileMode(QFileDialog::Directory);
            break;
    }

    if (file_dialog.exec()) {
        return file_dialog.selectedFiles()[0];
    }
    return QString();
}

void MainWindow::updateUiLock() {
    if (this->ui_lock) {
        // Menubar:File
        this->ui->actionAddFile->setEnabled(false);
        this->ui->actionAddDir->setEnabled(false);

        // Menubar:Database
        this->ui->actionLoadDatabase->setEnabled(true);
        this->ui->actionUnloadDatabase->setEnabled(false);
        this->ui->actionCheckDatabaseHashes->setEnabled(false);
        this->ui->actionCheckDatabasePaths->setEnabled(false);
        this->ui->actionRemoveDeletedEntries->setEnabled(false);

        // MainWindow:Buttons
        this->ui->pushButtonSearch->setEnabled(false);
        this->ui->pushButtonRandom->setEnabled(false);
        this->ui->pushButtonRefresh->setEnabled(false);

        // MainWindow:SearchInput
        this->ui->lineEditSearch->setEnabled(false);

        // Update ui lock status
        this->ui_lock = false;
    } else {
        // Menubar:File
        this->ui->actionAddFile->setEnabled(true);
        this->ui->actionAddDir->setEnabled(true);

        // Menubar:Database
        this->ui->actionLoadDatabase->setEnabled(false);
        this->ui->actionUnloadDatabase->setEnabled(true);
        this->ui->actionCheckDatabaseHashes->setEnabled(true);
        this->ui->actionCheckDatabasePaths->setEnabled(true);
        this->ui->actionRemoveDeletedEntries->setEnabled(true);

        // MainWindow:Buttons
        this->ui->pushButtonSearch->setEnabled(true);
        this->ui->pushButtonRandom->setEnabled(true);
        this->ui->pushButtonRefresh->setEnabled(true);

        // MainWindow:SearchInput
        this->ui->lineEditSearch->setEnabled(true);

        // Update ui lock status
        this->ui_lock = true;
    }
}

void MainWindow::setNoDatabaseStatus() {
    this->library_view_status->setText(QStringLiteral("No database loaded"));
}

void MainWindow::clearSearchText() {
    this->ui->lineEditSearch->setText("");
}

const QString MainWindow::getSearchText() {
    return this->ui->lineEditSearch->text();
}

void MainWindow::actionAddFile_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Add file"), QStringLiteral("Database busy"));
    } else {
        this->db_thread->start();
        emit request_getPathHashDatabaseData(false);
    }
}

void MainWindow::actionAddDir_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Add file"), QStringLiteral("Database busy"));
    } else {
        this->db_thread->start();
        emit request_getPathHashDatabaseData(true);
    }
}

void MainWindow::actionExit_triggered() {
    QApplication::quit();
}

// ZipWorker
void MainWindow::receive_ZipWorker_info(const QString &info) {
    QMessageBox::information(this, QStringLiteral("Add file(s)"), info);
}

void MainWindow::receive_ZipWorker_progress(int progress) {
    if (this->mainwindow_progress_dialog) {
        this->mainwindow_progress_dialog->setValue(progress);
    } else {
        this->mainwindow_progress_dialog = new QProgressDialog(QStringLiteral("Working on file(s)"), QStringLiteral("Cancel"), 0, 100, this, Qt::FramelessWindowHint);
        this->mainwindow_progress_dialog->setCursor(Qt::WaitCursor);
        this->mainwindow_progress_dialog->setAttribute(Qt::WA_DeleteOnClose);
        this->mainwindow_progress_dialog->setFixedSize(this->mainwindow_progress_dialog->size());
        this->mainwindow_progress_dialog->show();
        this->mainwindow_progress_dialog->activateWindow();
        this->mainwindow_progress_dialog->setValue(progress);
        // Progress dialog cancel
        connect(this->mainwindow_progress_dialog, &QProgressDialog::canceled, this, &MainWindow::main_window_progress_dialog_canceled);
    }
}

void MainWindow::receive_ZipWorker_data(QList<ZipData> data) {
    QMessageBox::information(this, QStringLiteral("Add file(s)"), QStringLiteral("Files to add in database: %1").arg(QString::number(data.length())));
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Database insert"), QStringLiteral("Action already running"));
    } else {
        this->db_thread->start();
        emit request_insertInDatabase(data);
    }
}

// DBWorker
void MainWindow::receive_DBWorker_info(const QString &info) {
    QMessageBox::information(this, QStringLiteral("Database"), info);
}

void MainWindow::receive_DBWorker_loadDatabase_status(bool status) {
    if (status) {
        this->updateUiLock();
        emit request_getAllDatabaseData();
        QMessageBox::information(this, QStringLiteral("Database"), QStringLiteral("Database loaded"));
    } else {
        this->db_thread->quit();
        QMessageBox::information(this, QStringLiteral("Database"), QStringLiteral("Database not loaded"));
    }
}

void MainWindow::receive_DBWorker_unloadDatabase_status(bool status) {
    if (status) {
        emit request_clearMangaList();
        this->clearSearchText();
        this->setNoDatabaseStatus();
        this->updateUiLock();
        QMessageBox::information(this, QStringLiteral("Database"), QStringLiteral("Database unloaded"));
    } else {
        QMessageBox::information(this, QStringLiteral("Database"), QStringLiteral("Could not unload the database"));
    }
}

void MainWindow::receive_DBWorker_progress(int progress) {
    if (this->mainwindow_progress_dialog) {
        this->mainwindow_progress_dialog->setValue(progress);
    } else {
        this->mainwindow_progress_dialog = new QProgressDialog(QStringLiteral("Working on file(s)"), QStringLiteral("Cancel"), 0, 100, this, Qt::FramelessWindowHint);
        this->mainwindow_progress_dialog->setCursor(Qt::WaitCursor);
        this->mainwindow_progress_dialog->setAttribute(Qt::WA_DeleteOnClose);
        this->mainwindow_progress_dialog->setFixedSize(this->mainwindow_progress_dialog->size());
        this->mainwindow_progress_dialog->show();
        this->mainwindow_progress_dialog->activateWindow();
        this->mainwindow_progress_dialog->setValue(progress);
        // Progress dialog cancel connect
        connect(this->mainwindow_progress_dialog, &QProgressDialog::canceled, this, &MainWindow::main_window_progress_dialog_canceled);
    }
}

void MainWindow::receive_DBWorker_data(QList<Manga> data) {
    Log::info(QStringLiteral("[DBWorker received]: %1").arg(QString::number(data.length())));
    emit request_setMangaList(data);
}

void MainWindow::receive_DBWorker_pathhash_data(QList<PathHash> data, bool is_dir) {
    if (this->zip_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Add file(s)"), QStringLiteral("Action already running"));
    } else if (is_dir == true) {
        QString dir_path = this->selectFile(FileDialog::SelectDir);
        if (Utils::Str::isNullOrEmpty(dir_path)) {
            return;
        }
        this->zip_thread->start();
        emit request_getDirJsonInfo(dir_path, data);
    } else if (is_dir == false) {
        QString file_path = this->selectFile(FileDialog::SelectZip);
        if (Utils::Str::isNullOrEmpty(file_path)) {
            return;
        }
        this->zip_thread->start();
        emit request_getFileJsonInfo(file_path, data);
    }
}

// LibraryView
void MainWindow::receive_LibraryView_deleteFromDatabse_request(QStringList hash_list) {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Delete from database"), QStringLiteral("Action already running"));
    } else {
        this->db_thread->start();
        emit request_deleteFromDatabase(hash_list);
    }
}

// Menubar:Database
void MainWindow::actionCreateDatabase_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Create database"), QStringLiteral("Action already running"));
    } else {
        QString file_path = this->selectFile(FileDialog::SelectDBSave);
        if (Utils::Str::isNullOrEmpty(file_path)) {
            return;
        }
        this->db_thread->start();
        emit request_createDatabase(file_path);
    }
}

void MainWindow::actionLoadDatabase_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Load database"), QStringLiteral("Action already running"));
    } else {
        QString file_path = this->selectFile(FileDialog::SelectDB);
        if (Utils::Str::isNullOrEmpty(file_path)) {
            return;
        }
        this->db_thread->start();
        emit request_loadDatabase(file_path);
    }
}

void MainWindow::actionUnloadDatabase_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Unload database"), QStringLiteral("Action already running"));
    } else {
        this->db_thread->start();
        emit request_unloadDatabase();
    }
}

void MainWindow::actionCheckDatabaseHashes_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Check database"), QStringLiteral("Action already running"));
    } else {
        this->db_thread->start();
        emit request_checkDatabaseHashes();
    }
}

void MainWindow::actionCheckDatabasePaths_triggered() {
    if (this->db_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Check database"), QStringLiteral("Action already running"));
    } else {
        this->db_thread->start();
        emit request_checkDatabaseFilepaths();
    }
}

// TODO: Menubar:Settings

// Buttons
void MainWindow::pushButtonSearch_clicked() {
    emit request_setSearchText(this->getSearchText());
}

void MainWindow::pushButtonRandom_clicked() {
    emit request_selectRandomManga();
}

void MainWindow::pushButtonRefresh_clicked() {
    this->clearSearchText();
    emit request_setSearchText("");
}

// Search
void MainWindow::lineEditSearch_returnPressed() {
    emit request_setSearchText(this->getSearchText());
}

// Progress dialog
void MainWindow::main_window_progress_dialog_canceled() {
    if(this->zip_thread->isRunning()) {
        this->zip_thread->requestInterruption();
    } else if (this->db_thread->isRunning()) {
        this->db_thread->requestInterruption();
    }
}

