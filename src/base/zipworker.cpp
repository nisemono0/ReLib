#include "base/zipworker.hpp"
#include "base/log.hpp"

#include "utils/fs.hpp"
#include "utils/str.hpp"
#include "utils/json.hpp"

#include <QThread>
#include <QtMinMax>
#include <QtConcurrent>
#include <QFuture>
#include <QJsonObject>
#include <QVariantMap>


ZipWorker::ZipWorker(QObject *parent) : QObject(parent) {

}

ZipWorker::~ZipWorker() {

}

ZipWorker::ZipWorkerInfo ZipWorker::getZipInfo(const QString &file_path, const QStringList &db_file_path_list) {
    ZipWorkerInfo ret_struct;

    if (db_file_path_list.contains(file_path)) {
        Log::warning(QStringLiteral("[File path already in database]: %1").arg(file_path));
        ret_struct.is_error = true;
        return ret_struct;
    }

    QJsonDocument json_doc = Utils::Zip::getInfoJsonDocument(file_path);
    if (Utils::Json::isNullOrEmptyJsonDocument(json_doc)) {
        Log::warning(QStringLiteral("[No info.json file (skipped)] %1").arg(file_path));
        ret_struct.is_error = true;
        return ret_struct;
    }
    if (!Utils::Json::isInfoJsonFormat(json_doc)) {
        Log::warning(QStringLiteral("[No json.info format (skipped)] %1").arg(file_path));
        ret_struct.is_error = true;
        return ret_struct;
    }

    ret_struct.zip_data.info_json = json_doc;
    ret_struct.zip_data.file_path = file_path;
    ret_struct.zip_data.file_hash = Utils::Fs::getSha251FromFile(file_path);
    ret_struct.is_error = false;

    return ret_struct;
}

void ZipWorker::receive_getFileJsonInfo_request(const QString &file_path, const QStringList &db_file_path_list) {
    if (! Utils::Fs::fileExists(file_path)) {
        emit send_ZipWorker_info(QStringLiteral("Empty filename"));
        return;
    }

    ZipWorkerInfo custom_ret = this->getZipInfo(file_path, db_file_path_list);

    if (custom_ret.is_error) {
        emit send_ZipWorker_data(QList<ZipData>());
        return;
    }

    QList<ZipData> zip_data_list = QList<ZipData>();

    Log::info(QStringLiteral("[ZipWorker:Add] %1").arg(custom_ret.zip_data.file_path));
    zip_data_list.append(custom_ret.zip_data);

    emit send_ZipWorker_data(zip_data_list);
}

void ZipWorker::receive_getDirJsonInfo_request(const QString &dir_path, const QStringList &db_file_path_list) {
    QStringList files_list = Utils::Fs::getDirZipList(dir_path);
    qsizetype files_list_len = files_list.length();

    QList<ZipData>zip_data_list = QList<ZipData>();

    int progress = 0;

    for (int file_idx = 0; file_idx < files_list_len; file_idx += BatchSize::Zip) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_ZipWorker_progress(100);
            // If list is empty send message, otherwise send the list
            if (zip_data_list.isEmpty()) {
                emit send_ZipWorker_info(QStringLiteral("Add dir canceled; Nothing to add"));
            } else {
                emit send_ZipWorker_data(zip_data_list);
            }
            return;
        }

        QStringList batch_file_list = QStringList();
        int current_batch_len = qMin(BatchSize::Zip, files_list_len - file_idx);
        for (int batch_idx = 0; batch_idx < current_batch_len; batch_idx++) {
            batch_file_list.append(files_list[file_idx + batch_idx]);
            progress = static_cast<int>((file_idx + batch_idx + 1) * 100.0 / files_list_len);
        }

        // Start (current_batch_len) concurrent threads getting info.json QJsonObject from zip files
        auto mapped_batch_results = QtConcurrent::blockingMapped(batch_file_list, [this, db_file_path_list](const QString &file_path) {
                return this->getZipInfo(file_path, db_file_path_list);
                });
        for (auto batch_results_it : mapped_batch_results) {
            if (!batch_results_it.is_error) {
                zip_data_list.append(batch_results_it.zip_data);
                Log::info(QStringLiteral("[ZipWorker:Add] %1").arg(batch_results_it.zip_data.file_path));
            }
        }

        emit send_ZipWorker_progress(progress);

    }

    emit send_ZipWorker_data(zip_data_list);
}

