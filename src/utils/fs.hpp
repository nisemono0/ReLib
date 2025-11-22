#pragma once

#include "utils/defs.hpp"

#include <QString>
#include <QStringList>


namespace Utils::Fs {
    // Check if file exists
    bool fileExists(const QString &file_path);
    // Return Sha256 of the first 512kb of a file
    QString getSha251FromFile(const QString &file_path);
    // Return a QStringList of zips in a directory
    QStringList getDirZipList(const QString &dir_path);
    // Return complete basename of file
    QString getCompleteBaseName(const QString &file_path);
    // Returns filesize of file
    QString getFileSize(const QString &file_path);
    // Returns the absolute path of a file (w/o the file name)
    // If dir/file doesn't exist it returns home path
    QString getAbsolutePath(const QString &path);
};

