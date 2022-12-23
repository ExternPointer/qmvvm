#include <QByteArray>
#include <QObject>
#include <QQmlEngine>

#include "QMvvm/Classes/ListModel.h"


ListModel::ListModel( std::unique_ptr<CollectionConnector> collection, QObject* parent )
    : QAbstractListModel( parent ) {
    this->m_collection = std::move( collection );
    this->m_collection->setHandlers( [ this ]( int start, int end ) { this->onBeginInserting( start, end ); }, [ this ]() { this->onEndInserting(); },
                                     [ this ]( int start, int end ) { this->onBeginRemoving( start, end ); }, [ this ]() { this->onEndRemoving(); },
                                     [ this ]( int start, int end ) { this->onEndDataChanging( start, end ); } );
}

QVariant ListModel::data( const QModelIndex& index, int role ) const {
    if( role != 0 && role != 1 ) {
        qWarning() << "invalid role id: " << role;
        return QVariant();
    }
    int count = this->m_collection->count();
    int row = index.row();
    if( row < 0 || row >= count ) {
        qWarning() << QString( "row is out of range [0,%0]: " ).arg( count - 1 ) << row;
        return QVariant();
    }
    if( role == 0 ) {
        return this->m_collection->at( row );
    }
    return this->m_collection->getRaw( row );
}

int ListModel::rowCount( const QModelIndex& parent ) const {
    Q_UNUSED( parent );
    return this->m_collection->count();
}

QHash<int, QByteArray> ListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ 0 ] = "modelData";
    roles[ 1 ] = "rawData";
    return roles;
}

void ListModel::onBeginInserting( int start, int end ) {
    this->beginInsertRows( this->m_parent, start, end );
}

void ListModel::onEndInserting() {
    this->endInsertRows();
}

void ListModel::onBeginRemoving( int start, int end ) {
    this->beginRemoveRows( this->m_parent, start, end );
}

void ListModel::onEndRemoving() {
    this->endRemoveRows();
}

void ListModel::onEndDataChanging( int start, int end ) {
    QModelIndex first = this->index( start );
    QModelIndex last = this->index( end );
    emit this->dataChanged( first, last );
}
