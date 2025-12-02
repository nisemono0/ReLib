#pragma once

#include <QSortFilterProxyModel>
#include <QCollator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>


class LibraryModelProxy : public QSortFilterProxyModel {
    Q_OBJECT;
public:
    explicit LibraryModelProxy(QObject *parent = nullptr);
    ~LibraryModelProxy();

    // Set the filter text
    void setSearchText(const QString &search_text);
    // Check if given index is filtered by the search or not
    bool isIndexFiltered(const QModelIndex &index);

    // Returns true if proxy has items displayed
    bool hasItems();

    // Return first visible index
    QModelIndex getFirstIndex();
    // Return last visible index
    QModelIndex getLastIndex();
    // Return random index from visible
    QModelIndex getRandomIndex();

private:
    QCollator *sort_collator;

    QString search_text;
    QRegularExpression search_regex;

    bool searchItemData(const QStringList &search_list, const QStringList &data_list) const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

signals:

public slots:
};

