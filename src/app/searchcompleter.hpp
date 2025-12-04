#pragma once

#include "base/manga.hpp"

#include <QObject>
#include <QCompleter>
#include <QStringListModel>
#include <QMap>
#include <QTextCursor>
#include <QTextEdit>


class SearchCompleter : public QCompleter {
    Q_OBJECT;
public:
    enum CompleterRole {
        Disabled  = Qt::UserRole + 1,
        Basename  = Qt::UserRole + 2,
        Title     = Qt::UserRole + 3,
        Artist    = Qt::UserRole + 4,
        Parody    = Qt::UserRole + 5,
        Circle    = Qt::UserRole + 6,
        Magazine  = Qt::UserRole + 7,
        Event     = Qt::UserRole + 8,
        Publisher = Qt::UserRole + 9,
        Tags      = Qt::UserRole + 10,
    };

    explicit SearchCompleter(QObject *parent = nullptr);
    ~SearchCompleter();

    // Sets the completer data based on the list of manga info
    void setCompleterData(const QList<Manga> &data);

    // Makes it so i can have autocomplete when having multiple tags
    virtual QStringList splitPath(const QString &path) const override;

private:
    QStringListModel *completer_model;
    QMap<SearchCompleter::CompleterRole, QStringList> completer_data;
    SearchCompleter::CompleterRole current_completer_role;

    // Checks if completion entry given role exists
    // The check is case insensitive
    bool hasEntry(SearchCompleter::CompleterRole role, const QString &entry);
    // Insert entry at given role
    void insertEntry(SearchCompleter::CompleterRole role, const QString &entry);
    // Set the model stringlist to the one coresponding to role
    void updateCompletionMode(SearchCompleter::CompleterRole role);

signals:

public slots:
    void receive_updateCompletionMode_request(SearchCompleter::CompleterRole role, const QString &prefix);

};

