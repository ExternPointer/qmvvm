#pragma once

#include <QAbstractListModel>
#include <QVariant>
#include <QVariantList>
#include <functional>
#include <memory>

#include "QMvvm/Classes/ObservableCollection.h"


class CollectionConnector;

class ListModel : public QAbstractListModel {
    Q_OBJECT

public:
    ListModel( std::unique_ptr<CollectionConnector> collection, QObject* parent = nullptr );
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    void onBeginInserting( int start, int end );
    void onEndInserting();
    void onBeginRemoving( int start, int end );
    void onEndRemoving();
    void onEndDataChanging( int start, int end );

    std::unique_ptr<CollectionConnector> m_collection;
    QModelIndex m_parent;
};

class CollectionConnector {
public:
    virtual ~CollectionConnector() {};
    virtual void setHandlers( std::function<void( int, int )> beginInsert, std::function<void()> endInsert, std::function<void( int, int )> beginRemove,
                              std::function<void()> endRemove, std::function<void( int, int )> dataChanged ) = 0;
    virtual int count() = 0;
    virtual QVariant at( int index ) = 0;
    virtual QVariant getRaw( int index ) = 0;
};

template<typename T>
class CollectionConnectorTemplated : public CollectionConnector, IObserver {
public:
    CollectionConnectorTemplated( std::shared_ptr<ObservableCollection<T>> collection ) {
        this->m_collection = collection;
        this->m_onBeginInsert = []( int, int ) {};
        this->m_onEndInsert = []() {};
        this->m_onBeginRemove = []( int, int ) {};
        this->m_onEndRemove = []() {};
        this->m_onDataChanged = []( int, int ) {};
        this->m_collection->AttachObserver( this );
    }

    ~CollectionConnectorTemplated() {
        this->m_collection->DetachObserver( this );
    }

    inline void setHandlers( std::function<void( int, int )> beginInsert, std::function<void()> endInsert, std::function<void( int, int )> beginRemove,
                             std::function<void()> endRemove, std::function<void( int, int )> dataChanged ) override {
        this->m_onBeginInsert = beginInsert;
        this->m_onEndInsert = endInsert;
        this->m_onBeginRemove = beginRemove;
        this->m_onEndRemove = endRemove;
        this->m_onDataChanged = dataChanged;
    }

    inline int count() override {
        return (int)this->m_collection->GetCollection().size();
    }

    inline QVariant at( int index ) override {
        return QVariant::fromValue( this->m_collection->GetCollection().at( index ) );
    }

    inline QVariant getRaw( int index ) override {
        return QVariant::fromValue( this->m_collection->GetCollection().at( index ) );
    }

private:
    inline void OnEvent( IObservable* source, std::shared_ptr<BaseObserverEvent> eventArgs ) override {
        Q_UNUSED( source );
        auto event = std::static_pointer_cast<CollectionChangedEvent<T>>( eventArgs );
        switch( event->m_action ) {
        case CollectionChangedAction::BeginAdd:
            this->m_onBeginInsert( (int)event->m_startingIndex, (int)( event->m_startingIndex + event->m_newItems.size() - 1 ) );
            break;
        case CollectionChangedAction::Add:
            this->m_onEndInsert();
            break;
        case CollectionChangedAction::BeginRemove:
            this->m_onBeginRemove( (int)event->m_startingIndex, (int)( event->m_startingIndex + event->m_oldItems.size() - 1 ) );
            break;
        case CollectionChangedAction::Remove:
            this->m_onEndRemove();
            break;
        case CollectionChangedAction::BeginReplace:
            break;
        case CollectionChangedAction::Replace:
            this->m_onDataChanged( (int)event->m_startingIndex, (int)( event->m_startingIndex + event->m_newItems.size() - 1 ) );
            break;
        default:
            qWarning() << "Unknown CollectionChangedEvent action: " << (int)event->m_action;
        }
    }

    std::shared_ptr<ObservableCollection<T>> m_collection;
    std::function<void( int, int )> m_onBeginInsert;
    std::function<void()> m_onEndInsert;
    std::function<void( int, int )> m_onBeginRemove;
    std::function<void()> m_onEndRemove;
    std::function<void( int, int )> m_onDataChanged;
};

template<>
inline QVariant CollectionConnectorTemplated<std::shared_ptr<QObject>>::at( int index ) {
    return QVariant::fromValue( this->m_collection->GetCollection().at( index ).get() );
}
