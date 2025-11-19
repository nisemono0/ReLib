#pragma once

#include "utils/defs.hpp"

#include <quazip/JlCompress.h>
#include <quazip/quazipfile.h>

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QString>
#include <QMap>



class ZipWorker : public QObject {
    Q_OBJECT;
public:
    explicit ZipWorker(QObject *parent = nullptr);
    ~ZipWorker();

private:
    // For use in getZipInfo with QtConcurrent
    struct ZipWorkerInfo {
        ZipData zip_data = ZipData(); // ZipData type
        bool is_error = false; // True if returns with error
    };
    ZipWorkerInfo getZipInfo(const QString &file_path, const QList<PathHash> &path_hash_list);

signals:
    void send_ZipWorker_info(const QString &info);
    void send_ZipWorker_progress(int progress);
    void send_ZipWorker_data(const QList<ZipData> &data);

public slots:
    void receive_getFileJsonInfo_request(const QString &file_path, const QList<PathHash> &path_hash_list);
    void receive_getDirJsonInfo_request(const QString &dir_path, const QList<PathHash> &path_hash_list);

};

