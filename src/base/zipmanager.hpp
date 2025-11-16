#pragma once

#include "utils/zip.hpp"

#include <quazip/JlCompress.h>
#include <quazip/quazipfile.h>

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QString>
#include <QMap>



class ZipManager : public QObject {
    Q_OBJECT;
public:
    explicit ZipManager(QObject *parent = nullptr);
    ~ZipManager();

private:
    // For use in getZipInfo with QtConcurrent
    struct ZipManagerInfo {
        ZipData zip_data; // ZipData type
        bool is_error; // True if returns with error
    };
    ZipManagerInfo getZipInfo(const QString &file_path);
signals:
    void send_ZipManager_info(const QString &status);
    void send_ZipManager_progress(int progress);
    void send_ZipManager_data(QList<ZipData> data);

public slots:
    void receive_getFileJsonInfo_request(const QString &file_path);
    // TODO(Maybe):
    // Make this also take a (file_path, file_hash) pair along the
    // dir_path and check if any file inside dir_path has the same
    // (file_path, file_hash) as the one given and skip them if yes
    void receive_getDirJsonInfo_request(const QString &dir_path);

};

