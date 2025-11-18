#pragma once

#include "utils/defs.hpp"
#include "utils/zip.hpp"
#include "base/manga.hpp"

#include <QSqlDatabase>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>


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
    QList<PathHash> getPathHashDatabaseData();

    void checkDatabaseFilehash(const PathHash &file);
    void checkDatabaseFilepath(const QString &file_path);

signals:
    void send_DBManager_info(const QString &info);
    void send_DBManager_loadDatabase_status(bool status);
    void send_DBManager_unloadDatabase_status(bool status);
    void send_DBManager_progress(int progress);
    void send_DBManager_data(QList<Manga> data);
    void send_DBManager_pathhash_data(QList<PathHash> data, bool is_dir);

public slots:
    void receive_createDatabase_request(const QString &file_path);
    void receive_loadDatabase_request(const QString &file_path);
    void receive_unloadDatabase_request();
    void receive_insertIntoDatabase_request(QList<ZipData> zip_data_list);
    void receive_deleteFromDatabase_request(QStringList hash_list);
    void receive_getAllDatabaseData_request();
    void receive_getPathHashDatabaseData_request(bool is_dir);
    void receive_checkDatabaseHashes_request();
    void receive_checkDatabaseFilepaths_request();
};

