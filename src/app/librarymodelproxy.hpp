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

    void setSearchText(const QString &search_text);

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

