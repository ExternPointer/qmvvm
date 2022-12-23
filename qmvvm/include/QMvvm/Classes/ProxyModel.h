#pragma once

#include <QObject>
#include <QSortFilterProxyModel>


class ProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

    Q_PROPERTY( QObject* filter MEMBER m_filter READ GetFilterObject WRITE SetFilterObject NOTIFY filterChanged );

public:
    explicit ProxyModel( QObject* parent = nullptr );

    void setSourceModel( QAbstractItemModel* sourceModel ) override;

    [[nodiscard]] QObject* GetFilterObject() const;
    void SetFilterObject( QObject* filterObject );

signals:
    void filterChanged();

protected:
    [[nodiscard]] bool filterAcceptsRow( int row, const QModelIndex& parent ) const override;

private:
    QObject* m_filter = nullptr;
};
