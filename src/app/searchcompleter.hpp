#pragma once

#include "base/manga.hpp"

#include <QObject>
#include <QCompleter>
#include <QStringListModel>
#include <QMap>


class SearchCompleter : public QCompleter {
    Q_OBJECT;
public:
    enum CompleterRole {
        Basename  = Qt::UserRole + 1,
        Title     = Qt::UserRole + 2,
        Artist    = Qt::UserRole + 3,
        Parody    = Qt::UserRole + 4,
        Circle    = Qt::UserRole + 5,
        Magazine  = Qt::UserRole + 6,
        Event     = Qt::UserRole + 7,
        Publisher = Qt::UserRole + 8,
        Tags      = Qt::UserRole + 9,
    };

    explicit SearchCompleter(QObject *parent = nullptr);
    ~SearchCompleter();

    // Sets the completer data based on the list of manga info
    void setCompleterData(const QList<Manga> &data);

private:
    QStringListModel *completer_model;
    QMap<SearchCompleter::CompleterRole, QStringList> completer_data;

    // Checks if completion entry given role exists
    // The check is case insensitive
    bool hasEntry(SearchCompleter::CompleterRole role, const QString &entry);
    // Insert entry at given role
    void insertEntry(SearchCompleter::CompleterRole role, const QString &entry);
    // Set the model stringlist to the one coresponding to role
    void updateCompletionMode(SearchCompleter::CompleterRole role);

signals:

public slots:
    void receive_updateCompletionMode_request(SearchCompleter::CompleterRole role, const QString &text);

};

