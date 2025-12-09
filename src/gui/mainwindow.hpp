#pragma once

#include "ui/ui_mainwindow.h"
#include "gui/logdialog.hpp"
#include "gui/mangainfodialog.hpp"

#include "app/imageview.hpp"
#include "app/libraryview.hpp"
#include "app/searchplaintextedit.hpp"

#include "base/dbworker.hpp"
#include "base/zipworker.hpp"
#include "base/shortcuts.hpp"

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
#include <QTimer>


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
    // Manga info dialog
    MangaInfoDialog *manga_info_dialog;
    // Image scale slider
    QSlider *scale_slider;
    QWidgetAction *scale_slider_action;
    // View settings group
    QActionGroup *view_mode_actiongroup;
    // Library view status label
    QLabel *libraryview_status;
    // Image view status label
    QLabel *imageview_status;

    // Database
    DBWorker *db_worker;
    QThread *db_thread;
    // Archive
    ZipWorker *zip_worker;
    QThread *zip_thread;

    // Search while typing timer
    QTimer *search_timer;
    QString current_search;

    // Shortcuts
    Shortcuts *shortcuts;

    // Open file dialog
    QString selectFile(const FileDialog::FileDialog DIALOG_TYPE);

    // Lock/unlock ui elements
    bool is_locked = false;
    void updateUiLock();

    // Set db status as unloaded
    void setNoDatabaseStatus();

    // Search input
    void clearSearchText();
    const QString getSearchText();
    bool isLastSearchedText(const QString &search_text);

    // Settings loading
    void updateUiSettings();

    // Init shortcuts
    void setupShortcuts();

protected:
    // Disable tab/shift+tab widget selection
    bool focusNextPrevChild(bool next) override;

signals:
    // ZipWorker
    void request_getFileJsonInfo(const QString &file_path, const QStringList &db_file_path_list);
    void request_getDirJsonInfo(const QString &dir_path, const QStringList &db_file_path_list);

    // DBWorker
    void request_createDatabase(const QString &file_path);
    void request_loadDatabase(const QString &file_path);
    void request_unloadDatabase();
    void request_insertInDatabase(const QList<ZipData> &zip_data_list);
    void request_deleteFromDatabase(const QStringList &hash_list);
    void request_getAllDatabaseData();
    void request_getPathDatabaseData(bool is_dir);
    void request_checkDatabaseHashes();
    void request_checkDatabaseFilepaths();

    // LibraryView
    void request_setMangaList(const QList<Manga> &manga_list);
    void request_clearLibraryView();
    void request_setSearchText(const QString &search_text);
    void request_selectRandomManga();

    // ImageView
    void request_clearImageView();
    void request_scaleAndFitImage();

    // Search
    void request_setCompleterData(const QList<Manga> &data);

public slots:
    // ZipWorker
    void receive_ZipWorker_info(const QString &info);
    void receive_ZipWorker_progress(int progress);
    void receive_ZipWorker_data(const QList<ZipData> &data);

    // DBWorker
    void receive_DBWorker_info(const QString &info);
    void receive_DBWorker_loadDatabase_status(bool status);
    void receive_DBWorker_unloadDatabase_status(bool status);
    void receive_DBWorker_progress(int progress);
    void receive_DBWorker_data(const QList<Manga> &data);
    void receive_DBWorker_path_data(const QStringList &data, bool is_dir);

    // LibraryView
    void receive_LibraryView_deleteFromDatabse_request(const QStringList &hash_list);

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

    // Menubar:Settings
    void actionScaleImage_toggled(bool checked);
    void scale_slider_valueChanged(int value);
    void scale_slider_actionTriggered(int action);
    void scale_slider_increment_shortcut();
    void scale_slider_decrement_shortcut();
    void view_mode_actiongroup_triggered(QAction *action);
    void actionAutocompleteSearch_toggled(bool checked);
    void actionSearchWhileTyping_toggled(bool checked);
    void actionSelectFirstAfterSearch_toggled(bool checked);
    void actionRememberSettings_toggled(bool checked);

    // Buttons
    void pushButtonSearch_clicked();
    void pushButtonRandom_clicked();
    void pushButtonRefresh_clicked();

    // Search
    void searchPlainTextEdit_returnPressed();
    void search_focus_input_shortcut();
    void search_unfocus_input_shortcut();
    void refresh_search_shortcut();

    // Progress dialog
    void main_window_progress_dialog_canceled();

    // Search while typing timer
    void search_while_typing_timeout();


};

