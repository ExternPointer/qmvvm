#include <QQmlEngine>

#include "QMvvm/Classes/ProxyModel.h"


ProxyModel::ProxyModel( QObject* parent )
    : QSortFilterProxyModel( parent ) {
}

void ProxyModel::setSourceModel( QAbstractItemModel* sourceModel ) {
    if( sourceModel == nullptr ) {
        qWarning() << "sourceModel can not be null";
        return;
    }

    QSortFilterProxyModel::setSourceModel( sourceModel );
}

QObject* ProxyModel::GetFilterObject() const {
    return this->m_filter;
}

void ProxyModel::SetFilterObject( QObject* filterObject ) {
    if( this->m_filter != filterObject ) {

        if( this->m_filter != nullptr ) {
            QObject::disconnect( this->m_filter, SIGNAL( invalidate() ), this, SLOT( invalidate() ) );
        }

        this->m_filter = filterObject;

        if( this->m_filter != nullptr ) {
            QObject::connect( this->m_filter, SIGNAL( invalidate() ), this, SLOT( invalidate() ) );
        }

        this->invalidate();
        emit this->filterChanged();
    }
}

bool ProxyModel::filterAcceptsRow( int row, const QModelIndex& parent ) const {
    bool result = true;

    if( this->m_filter == nullptr ) {
        return result;
    }

    auto index = this->sourceModel()->index( row, 0, parent );
    auto data = this->sourceModel()->data( index );

    QMetaObject::invokeMethod( this->GetFilterObject(), "isRowAcceptable", Q_RETURN_ARG( bool, result ), Q_ARG( QVariant, data ) );

    return result;
}
