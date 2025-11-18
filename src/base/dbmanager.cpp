#include "base/dbmanager.hpp"
#include "base/log.hpp"

#include "utils/defs.hpp"
#include "utils/str.hpp"
#include "utils/json.hpp"
#include "utils/fs.hpp"

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtMath>
#include <QThread>
#include <QtConcurrent>


DBManager::DBManager(QObject *parent) : QObject(parent) {
    this->database = QSqlDatabase::addDatabase(DBOptions::DBDriver, DBOptions::ConName);
}

DBManager::~DBManager() {
    if (this->database.isOpen()) {
        this->database.close();
    }
}

bool DBManager::isDatabaseLoaded() {
    if (this->database.isOpen()) {
        return true;
    }
    return false;
}

bool DBManager::openDatabase(const QString &file_path) {
    this->database.setDatabaseName(file_path);
    if (this->database.open()) {
        return true;
    }
    return false;
}

void DBManager::closeDatabase() {
    this->database.close();
}

bool DBManager::startTransaction() {
    return this->database.transaction();
}

bool DBManager::commitTransaction() {
    return this->database.commit();
}

bool DBManager::rollbackTransaction() {
    return this->database.rollback();
}

QString DBManager::getDatabaseLastError() {
    return this->database.lastError().text();
}

QList<Manga> DBManager::getAllDatabaseData() {
    if (!this->isDatabaseLoaded()) {
        Log::warning(QStringLiteral("[DBManager:getAllDatabaseData] Database not loaded"));
        return QList<Manga>();
    }

    QList<Manga> manga_list = QList<Manga>();

    QSqlQuery select_query = QSqlQuery(this->database);

    if (!select_query.prepare(DBOptions::SelectDBQuery)) {
        QString prepare_error = select_query.lastError().text();
        Log::error(QStringLiteral("[DBManager:select:query.prepare] %1").arg(prepare_error));
        return QList<Manga>();
    }

    if (!select_query.exec()) {
        QString query_error = select_query.lastError().text();
        Log::error(QStringLiteral("[DBManager:select:exec]: %1").arg(query_error));
        return QList<Manga>();
    }

    while (select_query.next()) {
        ZipData zip_data;
        zip_data.file_hash = select_query.value(0).toString();
        zip_data.file_path = select_query.value(1).toString();
        zip_data.info_json = QJsonDocument::fromJson(select_query.value(2).toByteArray());

        manga_list.append(Utils::Json::convertJsonInfoToManga(zip_data));
    }

    return manga_list;
}

void DBManager::checkDatabaseFilehash(const PathHash &file) {
    if (Utils::Fs::fileExists(file.file_path)) {
        if (Utils::Fs::getSha251FromFile(file.file_path) != file.file_hash) {
            Log::warning(QStringLiteral("[Filehash check] [Hashes differ]: %1").arg(file.file_path));
        }
    } else {
        Log::warning(QStringLiteral("[Filehash check] [File not found]: %1").arg(file.file_path));
    }
}

void DBManager::checkDatabaseFilepath(const QString &file_path) {
    if (!Utils::Fs::fileExists(file_path)){
        Log::warning(QStringLiteral("[Filepath check] [File not found]: %1").arg(file_path));
    }
}

void DBManager::receive_createDatabase_request(const QString &file_path) {
    if (Utils::Str::isNullOrEmpty(file_path)) {
        emit send_DBManager_info(QStringLiteral("Create database: empty filename"));
        return;
    }

    { // Hacky way clear db when out of scope
        QString error_text;

        QSqlDatabase db = QSqlDatabase::addDatabase(DBOptions::DBDriver, DBOptions::CreateConName);
        db.setDatabaseName(file_path);
        if(!db.open()) {
            error_text = db.lastError().text();
            emit send_DBManager_info(QStringLiteral("Create database open: %1").arg(error_text));
            return;
        }

        QSqlQuery create_query = QSqlQuery(db);

        if (!create_query.prepare(DBOptions::CreateDBQuery)) {
            error_text = create_query.lastError().text();
            emit send_DBManager_info(QStringLiteral("Create database prepare query: %1").arg(error_text));
            return;
        }

        if (!create_query.exec()) {
            error_text = create_query.lastError().text();
            emit send_DBManager_info(QStringLiteral("Create database exec query: %1").arg(error_text));
            return;
        }
        create_query.clear();
        db.commit();
        db.close();
    } // Scope end

    QSqlDatabase::removeDatabase(DBOptions::CreateConName);

    emit send_DBManager_info(QStringLiteral("Database created"));
}

void DBManager::receive_loadDatabase_request(const QString &file_path) {
    if (Utils::Str::isNullOrEmpty(file_path)){
        emit send_DBManager_info(QStringLiteral("Empty filename"));
        return;
    }

    if (this->isDatabaseLoaded()) {
        emit send_DBManager_info(QStringLiteral("Database already open"));
        return;
    }

    if (this->openDatabase(file_path)) {
        emit send_DBManager_loadDatabase_status(true);
        return;
    }

    emit send_DBManager_info(QStringLiteral("Database not loaded"));
}

void DBManager::receive_unloadDatabase_request() {
    if (this->isDatabaseLoaded()) {
        this->closeDatabase();
        emit send_DBManager_unloadDatabase_status(true);
        return;
    }
    emit send_DBManager_info(QStringLiteral("Could not unload the database"));
}

void DBManager::receive_insertIntoDatabase_request(QList<ZipData> zip_data_list) {
    if (!this->isDatabaseLoaded()) {
        emit send_DBManager_info(QStringLiteral("Database not loaded"));
        return;
    }

    int progress = 0;
    int total_inserted = 0;

    QSqlQuery insert_query = QSqlQuery(this->database);

    qsizetype zip_data_len = zip_data_list.length();
    for (int zip_data_idx = 0; zip_data_idx < zip_data_len; zip_data_idx += BatchSize::DBInsert) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_DBManager_progress(100);
            if (total_inserted == 0) {
                emit send_DBManager_info(QStringLiteral("Database insert interrupted; Nothing inserted"));
            } else {
                emit send_DBManager_info(QStringLiteral("Database insert interrupted; Inserted: %1").arg(total_inserted));
            }
            return;
        }

        // Start a transaction
        // If unsuccessful log the error and return
        if (!this->startTransaction()) {
            QString transaction_error = this->getDatabaseLastError();
            Log::error(QStringLiteral("[DBManager:insert:transaction] %1").arg(transaction_error));
            emit send_DBManager_progress(100);
            emit send_DBManager_info(QStringLiteral("Database insert transaction: %1").arg(transaction_error));
            return;
        }

        // Batch transaction
        int current_batch_len = qMin(BatchSize::DBInsert, zip_data_len - zip_data_idx);
        for (int batch_idx = 0; batch_idx < current_batch_len; batch_idx++) {
            QString file_hash = zip_data_list[zip_data_idx + batch_idx].file_hash;
            QString file_path = zip_data_list[zip_data_idx + batch_idx].file_path;
            QString info_json = zip_data_list[zip_data_idx + batch_idx].info_json.toJson(QJsonDocument::Compact);

            if (!insert_query.prepare(DBOptions::InsertDBQuery)) {
                QString prepare_error = insert_query.lastError().text();
                Log::error(QStringLiteral("[DBManager:insert:query.prepare] %1").arg(prepare_error));
                emit send_DBManager_info(QStringLiteral("Database insert query prepare: %1").arg(prepare_error));
                return;
            }

            insert_query.bindValue(QStringLiteral(":file_hash"), file_hash);
            insert_query.bindValue(QStringLiteral(":file_path"), file_path);
            insert_query.bindValue(QStringLiteral(":info_json"), info_json);

            // If query exec is unsuccessful log error and return
            if (!insert_query.exec()) {
                QString query_error = insert_query.lastError().text();
                Log::error(QStringLiteral("[DBManager:insert:exec]: %1").arg(query_error));
                emit send_DBManager_progress(100);
                emit send_DBManager_info(QStringLiteral("Database insert query exec: %1").arg(query_error));
                return;
            }

            // This seems retarded but it works to get the
            // number of entries inserted and to log only them
            int affected_rows = insert_query.numRowsAffected();
            if (affected_rows != 0) {
                Log::info(QStringLiteral("[DBManager:insert] %1").arg(file_path));
                total_inserted += affected_rows;
            }

            progress = static_cast<int>((zip_data_idx + batch_idx + 1) * 100.0 / zip_data_len);
        }


        // If commit transaction unsuccessful log error and return
        if (!this->commitTransaction()) {
            this->rollbackTransaction();
            QString commit_error = this->getDatabaseLastError();
            Log::error(QStringLiteral("[DBManager:insert:commit]: %1").arg(commit_error));
            emit send_DBManager_progress(100);
            emit send_DBManager_info(QStringLiteral("Database insert commit: %1").arg(commit_error));
            return;
        }

        emit send_DBManager_progress(progress);
    }

    emit send_DBManager_data(this->getAllDatabaseData());
}

void DBManager::receive_deleteFromDatabase_request(QStringList hash_list) {
    if (!this->isDatabaseLoaded()) {
        emit send_DBManager_info(QStringLiteral("Database not loaded"));
        return;
    }

    int progress = 0;
    int total_deleted = 0;

    QSqlQuery delete_query = QSqlQuery(this->database);

    qsizetype hash_list_len = hash_list.length();
    for (int hash_list_idx = 0; hash_list_idx < hash_list_len; hash_list_idx += BatchSize::DBDelete) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_DBManager_progress(100);
            if (total_deleted == 0) {
                emit send_DBManager_info(QStringLiteral("Database delete interrupted; Nothing deleted"));
            } else {
                emit send_DBManager_info(QStringLiteral("Database delete interrupted; Deleted: %1").arg(total_deleted));
            }
            return;
        }

        // Start transaction
        if (!this->startTransaction()) {
            QString transaction_error = this->getDatabaseLastError();
            Log::error(QStringLiteral("[DBManager:delete:transaction] %1").arg(transaction_error));
            emit send_DBManager_progress(100);
            emit send_DBManager_info(QStringLiteral("Database delete transaction: %1").arg(transaction_error));
            return;
        }

        // Batch delete
        int current_batch_len = qMin(BatchSize::DBDelete, hash_list_len);
        for (int batch_idx = 0; batch_idx < current_batch_len; batch_idx++) {
            QString file_hash = hash_list[hash_list_idx + batch_idx];

            if (!delete_query.prepare(DBOptions::DeleteDBQuery)) {
                QString prepare_error = delete_query.lastError().text();
                Log::error(QStringLiteral("[DBManager:delete:query.prepare] %1").arg(prepare_error));
                emit send_DBManager_info(QStringLiteral("Database delete query prepare: %1").arg(prepare_error));
                return;
            }

            delete_query.bindValue(QStringLiteral(":file_hash"), file_hash);

            if (!delete_query.exec()) {
                QString query_error = delete_query.lastError().text();
                Log::error(QStringLiteral("[DBManager:delete:exec]: %1").arg(query_error));
                emit send_DBManager_progress(100);
                emit send_DBManager_info(QStringLiteral("Database delete query exec: %1").arg(query_error));
                return;
            }

            int affected_rows = delete_query.numRowsAffected();
            if (affected_rows != 0) {
                Log::info(QStringLiteral("[DBManager:delete] %1").arg(file_hash));
                total_deleted += affected_rows;
            }

            progress = static_cast<int>((hash_list_idx + batch_idx + 1) * 100.0 / hash_list_len);
        }

        if (!this->commitTransaction()) {
            this->rollbackTransaction();
            QString commit_error = this->getDatabaseLastError();
            Log::error(QStringLiteral("[DBManager:delete:commit]: %1").arg(commit_error));
            emit send_DBManager_progress(100);
            emit send_DBManager_info(QStringLiteral("Database delete commit: %1").arg(commit_error));
            return;
        }

        emit send_DBManager_progress(progress);
    }

    emit send_DBManager_data(this->getAllDatabaseData());
}

void DBManager::receive_getAllDatabaseData_request() {
    emit send_DBManager_data(this->getAllDatabaseData());
}

void DBManager::receive_checkDatabaseHashes_request() {
    if (!this->isDatabaseLoaded()) {
        emit send_DBManager_info(QStringLiteral("Database not loaded"));
        return;
    }

    int progress = 0;
    int total_checked = 0;

    QList<Manga> database_data = this->getAllDatabaseData();

    qsizetype database_data_len = database_data.length();
    for (int database_data_idx = 0; database_data_idx < database_data_len; database_data_idx += BatchSize::DBHashCheck) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_DBManager_progress(100);
            if (total_checked == 0) {
                emit send_DBManager_info(QStringLiteral("Database hash check interrupted; Nothing checked"));
            } else {
                emit send_DBManager_info(QStringLiteral("Database hash check interrupted; Checked: %1").arg(QString::number(total_checked)));
            }
            return;
        }

        QList<PathHash> batch_path_hash_list = QList<PathHash>();
        int current_batch_len = qMin(BatchSize::DBHashCheck, database_data_len - database_data_idx);
        for (int batch_idx = 0; batch_idx < current_batch_len; batch_idx++) {
            PathHash file;
            file.file_hash = database_data[database_data_idx + batch_idx].file_hash;
            file.file_path = database_data[database_data_idx + batch_idx].file_path;
            batch_path_hash_list.append(file);
            progress = static_cast<int>((database_data_idx + batch_idx + 1) * 100.0 / database_data_len);
        }

        // No need to check for results since we only log
        QtConcurrent::blockingMap(batch_path_hash_list, [this](const PathHash &file) {
                this->checkDatabaseFilehash(file);
                });

        emit send_DBManager_progress(progress);
    }

    emit send_DBManager_info(QStringLiteral("File hash check done; Check logs window"));
}

void DBManager::receive_checkDatabaseFilepaths_request() {
    if (!this->isDatabaseLoaded()) {
        emit send_DBManager_info(QStringLiteral("Database not loaded"));
        return;
    }

    int progress = 0;
    int total_checked = 0;

    QList<Manga> database_data = this->getAllDatabaseData();

    qsizetype database_data_len = database_data.length();
    for (int database_data_idx = 0; database_data_idx < database_data_len; database_data_idx += BatchSize::DBPathCheck) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_DBManager_progress(100);
            if (total_checked == 0) {
                emit send_DBManager_info(QStringLiteral("Database path check interrupted; Nothing checked"));
            } else {
                emit send_DBManager_info(QStringLiteral("Database path check interrupted; Checked: %1").arg(QString::number(total_checked)));
            }
            return;
        }

        QStringList batch_filepath_list = QStringList();
        int current_batch_len = qMin(BatchSize::DBPathCheck, database_data_len - database_data_idx);
        for (int batch_idx = 0; batch_idx < current_batch_len; batch_idx++) {
            batch_filepath_list.append(database_data[database_data_idx + batch_idx].file_path);
            progress = static_cast<int>((database_data_idx + batch_idx + 1) * 100.0 / database_data_len);
        }

        // No need to check for results since we only log
        QtConcurrent::blockingMap(batch_filepath_list, [this](const QString &file_path){
                this->checkDatabaseFilepath(file_path);
                });

        emit send_DBManager_progress(progress);
    }

    emit send_DBManager_info(QStringLiteral("File path check done; Check logs window"));
}

