#include "gui/mainwindow.hpp"

#include "base/log.hpp"
#include "base/settings.hpp"

#include "utils/str.hpp"
#include "utils/fs.hpp"

#include <QMessageBox>
#include <QFileDialog>
#include <QToolTip>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // Ui setup
    this->ui->setupUi(this);

    this->log_dialog = new LogDialog(this);
    this->manga_info_dialog = new MangaInfoDialog(this);

    this->libraryview_status = new QLabel(this);
    this->ui->statusBar->addWidget(this->libraryview_status);
    this->libraryview_status->setIndent(4);
    this->imageview_status = new QLabel(this);
    this->ui->statusBar->addPermanentWidget(this->imageview_status);
    this->setNoDatabaseStatus();

    this->scale_slider_action = new QWidgetAction(this->ui->menuSettings);
    this->scale_slider = new QSlider(this->ui->menuSettings);
    this->scale_slider->setFixedHeight(25);
    this->scale_slider->setOrientation(Qt::Horizontal);
    this->scale_slider->setTickPosition(QSlider::NoTicks);
    this->scale_slider->setMaximum(10);
    this->scale_slider->setMinimum(0);
    this->scale_slider->setSingleStep(0);
    this->scale_slider->setPageStep(0);
    this->scale_slider->setValue(0);
    this->scale_slider->setToolTip(QStringLiteral("Scale: %1").arg(QString::number(Settings::image_scale_value)));
    this->scale_slider_action->setDefaultWidget(this->scale_slider);
    this->ui->menuSettings->insertAction(this->ui->menuSettingsView->menuAction(), this->scale_slider_action);
    this->ui->menuSettings->insertSeparator(this->ui->menuSettingsView->menuAction());

    this->view_mode_actiongroup = new QActionGroup(this->ui->menuSettingsView);
    this->view_mode_actiongroup->addAction(this->ui->actionFitInView);
    this->view_mode_actiongroup->addAction(this->ui->actionFitToWidth);
    this->view_mode_actiongroup->addAction(this->ui->actionFreeView);
    this->view_mode_actiongroup->setExclusive(true);

    // Thread setup
    this->zip_worker = new ZipWorker();
    this->zip_thread = new QThread(this);
    this->zip_worker->moveToThread(this->zip_thread);

    this->db_worker = new DBWorker();
    this->db_thread = new QThread(this);
    this->db_worker->moveToThread(this->db_thread);

    // Search while typing timer
    this->search_timer = new QTimer(this);
    this->search_timer->setInterval(Search::SearchTimer);
    if (Settings::search_while_typing) {
        this->search_timer->start();
    }

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

    // Menubar:Settings
    connect(this->ui->actionScaleImage, &QAction::toggled, this, &MainWindow::actionScaleImage_toggled);
    connect(this->scale_slider, &QSlider::valueChanged, this, &MainWindow::slider_scale_valueChanged);
    connect(this->view_mode_actiongroup, &QActionGroup::triggered, this, &MainWindow::view_mode_actiongroup_triggered);
    connect(this->ui->actionSearchWhileTyping, &QAction::toggled, this, &MainWindow::actionSearchWhileTyping_toggled);
    connect(this->ui->actionSelectFirstAfterSearch, &QAction::toggled, this, &MainWindow::actionSelectFirstAfterSearch_toggled);
    connect(this->ui->actionRememberSettings, &QAction::toggled, this, &MainWindow::actionRememberSettings_toggled);
    connect(this->ui->actionLoadLastDatabase, &QAction::toggled, this, &MainWindow::actionLoadLastDatabase_toggled);

    // Menubar:Info
    connect(this->ui->actionShowLogs, &QAction::triggered, this->log_dialog, &LogDialog::receive_showLogDialog_request);
    connect(this->ui->actionShowAbout, &QAction::triggered, this, &QApplication::aboutQt);

    // Buttons
    connect(this->ui->pushButtonSearch, &QPushButton::clicked, this, &MainWindow::pushButtonSearch_clicked);
    connect(this->ui->pushButtonRandom, &QPushButton::clicked, this, &MainWindow::pushButtonRandom_clicked);
    connect(this->ui->pushButtonRefresh, &QPushButton::clicked, this, &MainWindow::pushButtonRefresh_clicked);

    // Search
    connect(this->ui->lineEditSearch, &QLineEdit::returnPressed, this, &MainWindow::lineEditSearch_returnPressed);
    // Search while typing timer
    connect(this->search_timer, &QTimer::timeout, this, &MainWindow::search_while_typing_timeout);

    // Library view
    connect(this, &MainWindow::request_setMangaList, this->ui->libraryView, &LibraryView::receive_setMangaList_request);
    connect(this, &MainWindow::request_clearLibraryView, this->ui->libraryView, &LibraryView::receive_clearMangaList_request);
    connect(this, &MainWindow::request_setSearchText, this->ui->libraryView, &LibraryView::receive_setSearchText_request);
    connect(this, &MainWindow::request_selectRandomManga, this->ui->libraryView, &LibraryView::receive_selectRandomManga_request);
    connect(this->ui->libraryView, &LibraryView::send_LibraryView_status, this->libraryview_status, &QLabel::setText);
    connect(this->ui->libraryView, &LibraryView::send_LibraryView_deleteFromDatabase_request, this, &MainWindow::receive_LibraryView_deleteFromDatabse_request);
    connect(this->ui->libraryView, &LibraryView::request_showMangaInfoDialog, this->manga_info_dialog, &MangaInfoDialog::receive_showMangaInfoDialog_request);

    // Image view
    connect(this->ui->imageView, &ImageView::send_ImageView_status, this->imageview_status, &QLabel::setText);
    connect(this->ui->libraryView, &LibraryView::send_LibraryView_currentChanged_path, this->ui->imageView, &ImageView::receive_LibraryView_currentChanged_path);
    connect(this->ui->imageView, &ImageView::request_LibraryView_showMangaInfoDialog, this->ui->libraryView, &LibraryView::receive_showMangaInfoDialog_request);
    connect(this, &MainWindow::request_clearImageView, this->ui->imageView, &ImageView::receive_clearImageView_request);
    connect(this->ui->imageView, &ImageView::request_LibraryView_scrollToCurrentItem, this->ui->libraryView, &LibraryView::receive_scrollToCurrentItem_request);
    connect(this, &MainWindow::request_scaleAndFitImage, this->ui->imageView, &ImageView::receive_scaleAndFitImage_request);

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

    this->updateUiSettings();
    this->updateUiLock();
    this->setupShortcuts();
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
    delete this->scale_slider;
    delete this->scale_slider_action;

    delete this->libraryview_status;
    delete this->imageview_status;

    delete this->log_dialog;
    delete this->manga_info_dialog;

    delete this->shortcuts;

    delete this->ui;
}

QString MainWindow::selectFile(const FileDialog::FileDialog DIALOG_TYPE) {
    QFileDialog file_dialog = QFileDialog(this, QStringLiteral("Select file"));

    switch (DIALOG_TYPE) {
        case FileDialog::SelectDB:
            file_dialog.setDirectory(Settings::last_select_database_dialog_path);
            file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
            file_dialog.setFileMode(QFileDialog::ExistingFile);
            file_dialog.setNameFilter(QStringLiteral("SQLite files (*.db *.sqlite)"));
            break;
        case FileDialog::SelectDBSave:
            file_dialog.setDirectory(QDir::homePath());
            file_dialog.setAcceptMode(QFileDialog::AcceptSave);
            file_dialog.setFileMode(QFileDialog::AnyFile);
            file_dialog.setNameFilter(QStringLiteral("SQLite files (*.db *.sqlite)"));
            file_dialog.setDefaultSuffix(QStringLiteral(".db"));
            break;
        case FileDialog::SelectZip:
            file_dialog.setDirectory(Settings::last_add_file_dialog_path);
            file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
            file_dialog.setFileMode(QFileDialog::ExistingFile);
            file_dialog.setNameFilters({
                    QStringLiteral("Zip files (*.zip)"),
                    QStringLiteral("Cbz files (*.cbz)")
                    });
            break;
        case FileDialog::SelectDir:
            file_dialog.setDirectory(Settings::last_add_dir_dialog_path);
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
    // Menubar:File
    this->ui->actionAddFile->setEnabled(this->is_locked);
    this->ui->actionAddDir->setEnabled(this->is_locked);

    // Menubar:Database
    this->ui->actionLoadDatabase->setEnabled(!this->is_locked);
    this->ui->actionUnloadDatabase->setEnabled(this->is_locked);
    this->ui->actionCheckDatabaseHashes->setEnabled(this->is_locked);
    this->ui->actionCheckDatabasePaths->setEnabled(this->is_locked);
    this->ui->actionRemoveDeletedEntries->setEnabled(this->is_locked);

    // MainWindow:Buttons
    this->ui->pushButtonSearch->setEnabled(this->is_locked);
    this->ui->pushButtonRandom->setEnabled(this->is_locked);
    this->ui->pushButtonRefresh->setEnabled(this->is_locked);

    // MainWindow:SearchInput
    this->ui->lineEditSearch->setEnabled(this->is_locked);

    // Update ui lock status
    this->is_locked = !this->is_locked;
}

void MainWindow::setNoDatabaseStatus() {
    this->libraryview_status->setText(QStringLiteral("No database loaded"));
}

void MainWindow::clearSearchText() {
    this->ui->lineEditSearch->setText("");
    this->current_search = "";
}

const QString MainWindow::getSearchText() {
    this->current_search = this->ui->lineEditSearch->text();
    return this->current_search;
}

bool MainWindow::isLastSearchedText(const QString &search_text) {
    if (this->current_search == search_text) {
        return true;
    }
    return false;
}

void MainWindow::updateUiSettings() {
    if (Settings::remember_settings) {
        this->ui->actionScaleImage->setChecked(Settings::scale_image);
        this->scale_slider->setValue(Settings::scale_slider_value);
        this->scale_slider->setToolTip(QStringLiteral("Scale: %1").arg(QString::number(Settings::image_scale_value)));
        this->ui->actionFitInView->setChecked(true);
        this->ui->actionSearchWhileTyping->setChecked(Settings::search_while_typing);
        this->ui->actionSelectFirstAfterSearch->setChecked(Settings::select_first_item);
        this->ui->actionRememberSettings->setChecked(Settings::remember_settings);
        this->ui->actionLoadLastDatabase->setChecked(Settings::load_last_database);
    }
}

void MainWindow::setupShortcuts() {
    this->shortcuts = new Shortcuts(this);
    // LibraryView
    connect(this->shortcuts->libraryview_next_item, &QShortcut::activated, this->ui->libraryView, &LibraryView::receive_selectNextItem_shortcut);
    connect(this->shortcuts->libraryview_previous_item, &QShortcut::activated, this->ui->libraryView, &LibraryView::receive_selectPreviousItem_shortcut);
    connect(this->shortcuts->libraryview_first_item, &QShortcut::activated, this->ui->libraryView, &LibraryView::receive_selectFirstItem_shortcut);
    connect(this->shortcuts->libraryview_last_item, &QShortcut::activated, this->ui->libraryView, &LibraryView::receive_selectLastItem_shortcut);
    // ImageView
    connect(this->shortcuts->imageview_next_image, &QShortcut::activated, this->ui->imageView, &ImageView::receive_showNextImage_shortcut);
    connect(this->shortcuts->imageview_previous_image, &QShortcut::activated, this->ui->imageView, &ImageView::receive_showPreviousImage_shortcut);
    connect(this->shortcuts->imageview_load_images, &QShortcut::activated, this->ui->imageView, &ImageView::receive_loadImages_shortcut);
    // Search inpushortcuts->t
    connect(this->shortcuts->focus_search_input, &QShortcut::activated, this->ui->lineEditSearch, [=](){ this->ui->lineEditSearch->setFocus(); });
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

void MainWindow::receive_ZipWorker_data(const QList<ZipData> &data) {
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
        emit request_clearLibraryView();
        emit request_clearImageView();
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

void MainWindow::receive_DBWorker_data(const QList<Manga> &data) {
    Log::info(QStringLiteral("[DBWorker received]: %1").arg(QString::number(data.length())));
    emit request_setMangaList(data);
}

void MainWindow::receive_DBWorker_pathhash_data(const QList<PathHash> &data, bool is_dir) {
    if (this->zip_thread->isRunning()) {
        QMessageBox::information(this, QStringLiteral("Add file(s)"), QStringLiteral("Action already running"));
    } else if (is_dir == true) {
        QString dir_path = this->selectFile(FileDialog::SelectDir);
        if (Utils::Str::isNullOrEmpty(dir_path)) {
            return;
        }
        Settings::last_add_dir_dialog_path = Utils::Fs::getAbsolutePath(dir_path);
        this->zip_thread->start();
        emit request_getDirJsonInfo(dir_path, data);
    } else if (is_dir == false) {
        QString file_path = this->selectFile(FileDialog::SelectZip);
        if (Utils::Str::isNullOrEmpty(file_path)) {
            return;
        }
        Settings::last_add_file_dialog_path = Utils::Fs::getAbsolutePath(file_path);
        this->zip_thread->start();
        emit request_getFileJsonInfo(file_path, data);
    }
}

// LibraryView
void MainWindow::receive_LibraryView_deleteFromDatabse_request(const QStringList &hash_list) {
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
        Settings::last_select_database_dialog_path = Utils::Fs::getAbsolutePath(file_path);
        Settings::last_database_path = file_path;
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

// Menubar:Settings
void MainWindow::actionScaleImage_toggled(bool checked) {
    Settings::scale_image = checked;
    emit request_scaleAndFitImage();
}

void MainWindow::slider_scale_valueChanged(int value) {
    Settings::scale_slider_value = value;
    Settings::updateImageScaleValue();
    QToolTip::showText(
            QCursor::pos(),
            QStringLiteral("Scale: %1").arg(QString::number(Settings::image_scale_value)),
            this->scale_slider
            );
    this->scale_slider->setToolTip(QStringLiteral("Scale: %1").arg(QString::number(Settings::image_scale_value)));
    emit request_scaleAndFitImage();
}

void MainWindow::view_mode_actiongroup_triggered(QAction *action) {
    if (action == this->ui->actionFitInView) {
        Settings::imageview_option = ImageOptions::FitInView;
    } else if (action == this->ui->actionFitToWidth) {
        Settings::imageview_option = ImageOptions::FitToWidth;
    } else if (action == this->ui->actionFreeView) {
        Settings::imageview_option = ImageOptions::FreeView;
    } else {
        Settings::imageview_option = ImageOptions::FitInView;
    }
    emit request_scaleAndFitImage();
}

void MainWindow::actionSearchWhileTyping_toggled(bool checked) {
    Settings::search_while_typing = checked;
    if (checked) {
        this->search_timer->start();
    } else {
        this->search_timer->stop();
    }
}

void MainWindow::actionSelectFirstAfterSearch_toggled(bool checked) {
    Settings::select_first_item = checked;
}

void MainWindow::actionRememberSettings_toggled(bool checked) {
    Settings::remember_settings = checked;
}

void MainWindow::actionLoadLastDatabase_toggled(bool checked) {
    Settings::load_last_database = checked;
}

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
    this->ui->libraryView->setFocus();
}

// Progress dialog
void MainWindow::main_window_progress_dialog_canceled() {
    if (this->zip_thread->isRunning()) {
        this->zip_thread->requestInterruption();
    } else if (this->db_thread->isRunning()) {
        this->db_thread->requestInterruption();
    }
}

// Search while typing timer
void MainWindow::search_while_typing_timeout() {
    QString new_search = this->ui->lineEditSearch->text();
    if (!this->isLastSearchedText(new_search)) {
        this->current_search = new_search;
        emit request_setSearchText(new_search);
    }
}

