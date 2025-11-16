#pragma once

#include "ui/ui_mainwindow.h"
#include "gui/logdialog.hpp"

#include "app/imageview.hpp"
#include "app/libraryview.hpp"

#include "base/dbmanager.hpp"
#include "base/zipmanager.hpp"

#include "utils/defs.hpp"

#include <QMainWindow>
#include <QActionGroup>
#include <QWidget>
#include <QSlider>
#include <QWidgetAction>
#include <QDir>
#include <QThread>
#include <QString>
#include <QProgressDialog>
#include <QPointer>
#include <QVariantMap>
#include <QList>
#include <QLabel>


namespace Ui {
    class MainWindow;
};

class MainWindow : public QMainWindow {
    Q_OBJECT;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    // Progress dialog
    QPointer<QProgressDialog> mainwindow_progress_dialog;
    // Logs dialog
    LogDialog *log_dialog;
    // Image scale slider
    QSlider *slider_scale;
    QWidgetAction *action_scale_slider;
    // View settings group
    QActionGroup *actiongroup_view_mode;
    // Library view status label
    QLabel *library_view_status;

    // Database
    DBManager *db_manager;
    QThread *db_thread;
    // Archive
    ZipManager *zip_manager;
    QThread *zip_thread;

    // Open file dialog
    QString selectFile(const FileDialog::FileDialog DIALOG_TYPE);

    // Lock/unlock ui elements
    bool ui_lock = true;
    void updateUiLock();

    // Set db status as unloaded
    void setNoDatabaseStatus();

    // Search input
    void clearSearchText();
    const QString getSearchText();

signals:
    // ZipManager
    void request_getFileJsonInfo(const QString &file_path);
    void request_getDirJsonInfo(const QString &dir_path);

    // DBManager
    void request_createDatabase(const QString &file_path);
    void request_loadDatabase(const QString &file_path);
    void request_unloadDatabase();
    void request_insertInDatabase(QList<ZipData> zip_data_list);
    void request_deleteFromDatabase(QStringList hash_list);
    void request_getAllDatabaseData();
    void request_checkDatabaseHashes();
    void request_checkDatabaseFilepaths();

    // LibraryView
    void request_setMangaList(QList<Manga> manga_list);
    void request_clearMangaList();
    void request_setSearchText(const QString &search_text);

public slots:
    // ZipManager
    void receive_ZipManager_info(const QString &info);
    void receive_ZipManager_progress(int progress);
    void receive_ZipManager_data(QList<ZipData> data);

    // DBManager
    void receive_DBManager_info(const QString &info);
    void receive_DBManager_loadDatabase_status(bool status);
    void receive_DBManager_unloadDatabase_status(bool status);
    void receive_DBManager_progress(int progress);
    void receive_DBManager_data(QList<Manga> data);

private slots:
    // Menubar:File
    void actionAddFile_triggered();
    void actionAddDir_triggered();
    void actionExit_triggered();

    // Menubar:Databse
    void actionCreateDatabase_triggered();
    void actionLoadDatabase_triggered();
    void actionUnloadDatabase_triggered();
    void actionCheckDatabaseHashes_triggered();
    void actionCheckDatabasePaths_triggered();

    // TODO: Menubar:Settings

    // Menubar:Info
    void actionShowAbout_triggered();

    // Buttons
    void pushButtonSearch_clicked(bool checked);
    void pushButtonRandom_clicked(bool checked);
    void pushButtonRefresh_clicked(bool checked);

    // Search
    void lineEditSearch_returnPressed();

    // Progress dialog
    void main_window_progress_dialog_canceled();
};

