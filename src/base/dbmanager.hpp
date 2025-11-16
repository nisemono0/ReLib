#pragma once

#include "utils/defs.hpp"
#include "utils/zip.hpp"
#include "base/manga.hpp"

#include <QSqlDatabase>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>
#include <QPair>


class DBManager : public QObject {
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = nullptr);
    ~DBManager();

private:
    QSqlDatabase database;

    bool isDatabaseLoaded();
    bool openDatabase(const QString &file_path);
    void closeDatabase();

    bool startTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    QString getDatabaseLastError();

    QList<Manga> getAllDatabaseData();

    void checkDatabaseFilehash(const QPair<QString, QString> &path_hash_pair); // TODO: Refactor this to use a struct SoonTM
    void checkDatabaseFilepath(const QString &file_path);

signals:
    void send_DBManager_info(const QString &info);
    void send_DBManager_loadDatabase_status(bool status);
    void send_DBManager_unloadDatabase_status(bool status);
    void send_DBManager_progress(int progress);
    void send_DBManager_data(QList<Manga> data);

public slots:
    void receive_createDatabase_request(const QString &file_path);
    void receive_loadDatabase_request(const QString &file_path);
    void receive_unloadDatabase_request();
    void receive_insertIntoDatabase_request(QList<ZipData> zip_data_list);
    void receive_deleteFromDatabase_request(QStringList hash_list);
    void receive_getAllDatabaseData_request();
    void receive_checkDatabaseHashes_request();
    void receive_checkDatabaseFilepaths_request();
};

