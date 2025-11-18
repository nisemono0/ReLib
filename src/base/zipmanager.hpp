#pragma once

#include "utils/defs.hpp"

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
    ZipManagerInfo getZipInfo(const QString &file_path, const QList<PathHash> &path_hash_list);
signals:
    void send_ZipManager_info(const QString &status);
    void send_ZipManager_progress(int progress);
    void send_ZipManager_data(QList<ZipData> data);

public slots:
    void receive_getFileJsonInfo_request(const QString &file_path, const QList<PathHash> path_hash_list);
    void receive_getDirJsonInfo_request(const QString &dir_path, const QList<PathHash> path_hash_list);

};

