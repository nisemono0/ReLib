#include "utils/fs.hpp"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QLocale>


bool Utils::Fs::fileExists(const QString &file_path) {
    if (QFile::exists(file_path)) {
        return true;
    }
    return false;
}

QString Utils::Fs::getSha251FromFile(const QString &file_path) {
    QByteArray file_hash = QByteArray();

    QFile file = QFile(file_path);

    if (file.open(QIODevice::ReadOnly)) {
        file_hash = QCryptographicHash::hash(file.read(512 * 1024), QCryptographicHash::Sha256);
    }

    return QLatin1String(file_hash.toHex());
}

QStringList Utils::Fs::getDirZipList(const QString &dir_path) {
    QStringList zip_list = QStringList();

    QDir dir = QDir(dir_path);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters({ QStringLiteral("*.zip") });

    QDirIterator dir_it = QDirIterator(dir, QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while (dir_it.hasNext()) {
        zip_list.append(dir_it.next());
    }

    return zip_list;
}

QString Utils::Fs::getCompleteBaseName(const QString &file_path) {
    QFileInfo file_info = QFileInfo(file_path);
    return file_info.completeBaseName();
}

QString Utils::Fs::getFileSize(const QString &file_path) {
    QFileInfo file_info = QFileInfo(file_path);
    if (file_info.exists()) {
        QLocale locale_filesize = QLocale();
        return locale_filesize.formattedDataSize(file_info.size(), 1, QLocale::DataSizeIecFormat);
    }
    return QStringLiteral("File not found");
}

QString Utils::Fs::getAbsolutePath(const QString &path) {
    QFileInfo file_info = QFileInfo(path);
    if (file_info.exists()) {
        return file_info.absolutePath();
    }
    return QDir::homePath();
}

