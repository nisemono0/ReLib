#pragma once

#include "utils/defs.hpp"
#include "utils/zip.hpp"
#include "base/manga.hpp"

#include <QSqlDatabase>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QList>


class DBWorker : public QObject {
    Q_OBJECT
public:
    explicit DBWorker(QObject *parent = nullptr);
    ~DBWorker();

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
    QStringList getPathDatabaseData();

    void checkDatabaseFilehash(const PathHash &file);
    void checkDatabaseFilepath(const QString &file_path);

signals:
    void send_DBWorker_info(const QString &info);
    void send_DBWorker_loadDatabase_status(bool status);
    void send_DBWorker_unloadDatabase_status(bool status);
    void send_DBWorker_progress(int progress);
    void send_DBWorker_data(const QList<Manga> &data);
    void send_DBWorker_path_data(const QStringList &data, bool is_dir);

public slots:
    void receive_createDatabase_request(const QString &file_path);
    void receive_loadDatabase_request(const QString &file_path);
    void receive_unloadDatabase_request();
    void receive_insertIntoDatabase_request(const QList<ZipData> &zip_data_list);
    void receive_deleteFromDatabase_request(const QStringList &hash_list);
    void receive_getAllDatabaseData_request();
    void receive_getPathDatabaseData_request(bool is_dir);
    void receive_checkDatabaseHashes_request();
    void receive_checkDatabaseFilepaths_request();
};

