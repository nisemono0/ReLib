#pragma once

#include <Qt>
#include <QString>
#include <QJsonDocument>

// Custom data types
struct ZipData {
    QString file_hash;
    QString file_path;
    QJsonDocument info_json;
};

namespace App {
    static const QString AppName = QStringLiteral("ReLib");
    static const QString Style   = QStringLiteral("Fusion");
};

namespace ImageOptions {
    enum ImageOptions { FitInView, FitToWidth, FreeView };
};

namespace DBOptions {
    static const QString DBDriver      = QStringLiteral("QSQLITE");
    static const QString CreateConName = QStringLiteral("ReLib_CREATE_DB_CON");
    static const QString ConName       = QStringLiteral("ReLib_DB_CON");
    static const QString TableName     = QStringLiteral("ReLib");
    // CREATE DB QUERY
    static const QString CreateDBQuery = QStringLiteral(
            "CREATE TABLE IF NOT EXISTS %1 "
            "(id INTEGER NOT NULL PRIMARY KEY, "
            "file_hash TEXT NOT NULL UNIQUE, "
            "file_path TEXT NOT NULL UNIQUE, "
            "info_json TEXT);").arg(DBOptions::TableName);
    // INSERT UPDATE
    // IF file_hash exists, update file_path, info_json only if they differ from the ones inserted
    // IF file_path exists, update file_hash, info_json only if they differ from the ones inserted
    static const QString InsertDBQuery = QStringLiteral(
            "INSERT INTO %1 (file_hash, file_path, info_json) "
            "VALUES (:file_hash, :file_path, :info_json) "
            "ON CONFLICT (file_hash) DO UPDATE SET "
            "file_path=excluded.file_path, info_json=excluded.info_json "
            "WHERE excluded.file_path!=file_path OR excluded.info_json!=info_json "
            "ON CONFLICT (file_path) DO UPDATE SET "
            "file_hash=excluded.file_hash, info_json=excluded.info_json "
            "WHERE excluded.file_hash!=file_hash OR excluded.info_json!=info_json;").arg(DBOptions::TableName);
    // DELETE from database on file_hash
    static const QString DeleteDBQuery = QStringLiteral(
            "DELETE FROM %1 "
            "WHERE file_hash=:file_hash;").arg(DBOptions::TableName);
    // SELECt all besides primary key from database
    static const QString SelectDBQuery = QStringLiteral(
            "SELECT file_hash, file_path, info_json FROM %1;").arg(DBOptions::TableName);
};

namespace FileDialog {
    enum FileDialog { SelectDB, SelectDBSave, SelectZip, SelectDir };
};

namespace ZipInfo {
    static const QString InfoJson = QStringLiteral("info.json");
};

namespace BatchSize {
    static const int Zip         = 3;
    static const int DBInsert    = 100;
    static const int DBDelete    = 100;
    static const int DBHashCheck = 10;
    static const int DBPathCheck = 100;
};

