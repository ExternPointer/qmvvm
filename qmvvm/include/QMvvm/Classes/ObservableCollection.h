#pragma once

#include <QObject>
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <vector>

#include "QMvvm/Interfaces/IObservable.h"


enum class CollectionChangedAction : int {
    BeginAdd,
    Add,
    BeginRemove,
    Remove,
    BeginReplace,
    Replace,
};

template<typename T>
class CollectionChangedEvent : public BaseObserverEvent {
public:
    CollectionChangedEvent( const CollectionChangedAction& action, size_t startingIndex, const std::vector<T>& newItems = std::vector<T>(),
                            const std::vector<T>& oldItems = std::vector<T>() )
        : m_action( action )
        , m_newItems( newItems )
        , m_oldItems( oldItems )
        , m_startingIndex( startingIndex ) {
    }
    CollectionChangedAction m_action;
    std::vector<T> m_newItems;
    std::vector<T> m_oldItems;
    size_t m_startingIndex;
};

template<typename T>
class ObservableCollection : public IObservable {
public:
    ObservableCollection() = default;

    ObservableCollection( const T* array, size_t size ) {
        this->m_collection.reserve( size );
        std::copy( array, array + size, std::back_inserter( this->m_collection ) );
    }

    explicit ObservableCollection( const std::vector<T>& collection ) {
        this->m_collection = collection;
    }

    inline void AttachObserver( IObserver* observer ) override {
        this->m_observers.insert( observer );
    }

    inline void DetachObserver( IObserver* observer ) override {
        this->m_observers.erase( observer );
    }

    inline void InsertRange( size_t index, const std::vector<T>& collection ) {
        if( index > this->m_collection.size() ) {
            throw std::out_of_range( "ObservableCollection::InsertRange: Exception occured. Index out of range" );
        }

        auto beginInsertEvent = std::make_shared<CollectionChangedEvent<T>>( CollectionChangedAction::BeginAdd, index, collection );
        this->NotifyObservers( beginInsertEvent );

        for( std::size_t i = 0; i < collection.size(); i++ ) {
            this->m_collection.insert( this->m_collection.begin() + index + i, collection[ i ] );
        }

        auto endInsertEvent = std::make_shared<CollectionChangedEvent<T>>( CollectionChangedAction::Add, index, collection );
        this->NotifyObservers( endInsertEvent );
    }

    inline void RemoveRange( const std::vector<T>& collection ) {
        std::vector<T> collectionCopy = this->m_collection;
        std::unordered_map<int, std::vector<T>> clusters;
        std::queue<int> removeQueue;
        int lastIndex = -1;
        for( std::size_t i = 0; i < collection.size(); i++ ) {
            T item = collection[ i ];
            int index = std::find( collectionCopy.begin(), collectionCopy.end(), item ) - collectionCopy.begin();
            if( index < 0 ) {
                continue;
            }
            collectionCopy.erase( collectionCopy.begin() + index );
            if( lastIndex == index ) {
                clusters[ lastIndex ].push_back( item );
            } else {
                std::vector<T> cluster;
                cluster.push_back( item );
                clusters[ lastIndex = index ] = cluster;
                removeQueue.push( index );
            }
        }

        while( !removeQueue.empty() ) {
            int index = removeQueue.front();
            removeQueue.pop();
            this->RemoveRange( index, clusters[ index ].size() );
        }
    }

    inline void RemoveRange( size_t index, size_t count ) {
        if( index >= this->m_collection.size() || count > this->m_collection.size() || index + count > this->m_collection.size() ) {
            throw std::out_of_range( "ObservableCollection::InsertRange: Exception occured. Index out of range" );
        }

        std::vector<T> oldItems( this->m_collection.begin() + index, this->m_collection.begin() + index + count );

        auto beginRemoveEvent = std::make_shared<CollectionChangedEvent<T>>( CollectionChangedAction::BeginRemove, index, std::vector<T>(), oldItems );
        this->NotifyObservers( beginRemoveEvent );

        this->m_collection.erase( this->m_collection.begin() + index, this->m_collection.begin() + index + count );

        auto endRemoveEvent = std::make_shared<CollectionChangedEvent<T>>( CollectionChangedAction::Remove, index, std::vector<T>(), oldItems );
        this->NotifyObservers( endRemoveEvent );
    }

    inline void RemoveAll() {
        if( this->m_collection.size() == 0 ) {
            return;
        }
        this->RemoveRange( 0, this->m_collection.size() );
    }

    inline void ReplaceRange( size_t index, const std::vector<T>& collection ) {
        if( index >= this->m_collection.size() || collection.size() > this->m_collection.size() || index + collection.size() > this->m_collection.size() ) {
            throw std::out_of_range( "ObservableCollection::InsertRange: Exception occured. Index out of range" );
        }

        size_t count = collection.size();
        std::vector<T> oldItems = std::vector<T>( this->m_collection.begin() + index, this->m_collection.begin() + index + count );

        auto beginReplaceEvent = std::make_shared<CollectionChangedEvent<T>>( CollectionChangedAction::BeginReplace, index, collection, oldItems );
        this->NotifyObservers( beginReplaceEvent );

        for( int i = 0; i < count; i++ ) {
            this->m_collection[ index + i ] = collection[ i ];
        }
        auto replaceEvent = std::make_shared<CollectionChangedEvent<T>>( CollectionChangedAction::Replace, index, collection, oldItems );
        this->NotifyObservers( replaceEvent );
    }

    inline void ReplaceRange( size_t index, size_t count, const std::vector<T>& collection ) {
        if( collection.empty() ) {
            this->RemoveRange( index, count );
        } else if( count == 0 ) {
            this->InsertRange( index, collection );
        } else if( count < collection.size() ) {
            this->ReplaceRange( index, std::vector<T>( collection.begin(), collection.begin() + count ) );
            this->InsertRange( index + count, std::vector<T>( collection.begin() + count, collection.end() ) );
        } else if( count == collection.size() ) {
            this->ReplaceRange( index, collection );
        } else {
            this->ReplaceRange( index, collection );
            this->RemoveRange( index + collection.size(), count - collection.size() );
        }
    }

    inline void TransformTo( const std::vector<T>& collection ) {
        if( collection.empty() && this->m_collection.size() != 0 ) {
            this->RemoveAll();
            return;
        }

        if( this->m_collection.size() > collection.size() ) {
            this->TransformByRemoving( collection );
        } else if( this->m_collection.size() < collection.size() ) {
            this->TransformByInserting( collection );
        } else {
            this->TranformByReplacing( collection );
        }

        if( this->m_collection.size() > collection.size() ) {
            this->RemoveRange( collection.size(), this->m_collection.size() - collection.size() );
        }
        if( this->m_collection.size() < collection.size() ) {
            this->InsertRange( this->m_collection.size(), std::vector<T>( collection.begin() + this->m_collection.size(), collection.end() ) );
        }
    }

    inline const std::vector<T>& GetCollection() {
        return this->m_collection;
    }

private:
    inline void TransformByRemoving( const std::vector<T>& collection ) {
        size_t minCount = std::min( this->m_collection.size(), collection.size() );
        for( size_t i = 0; i < minCount; i++ ) {
            if( this->m_collection[ i ] != collection[ i ] ) {
                size_t startRemove = i;
                size_t endRemove = this->m_collection.size();

                for( size_t j = i; j < this->m_collection.size(); j++ ) {
                    if( this->m_collection[ j ] == collection[ i ] ) {
                        endRemove = j;
                        break;
                    }
                }

                this->RemoveRange( startRemove, endRemove - startRemove );
                minCount = std::min( this->m_collection.size(), collection.size() );
            }
        }
    }

    inline void TransformByInserting( const std::vector<T>& collection ) {
        size_t minCount = std::min( this->m_collection.size(), collection.size() );
        for( size_t i = 0; i < minCount; i++ ) {
            if( this->m_collection[ i ] != collection[ i ] ) {
                std::vector<T> itemsToInsert;
                size_t startInsert = i;
                for( size_t j = i; j < collection.size(); j++ ) {
                    if( this->m_collection[ i ] == collection[ j ] ) {
                        break;
                    }
                    itemsToInsert.push_back( collection[ j ] );
                }
                this->InsertRange( startInsert, itemsToInsert );
                minCount = std::min( this->m_collection.size(), collection.size() );
            }
        }
    }

    inline void TranformByReplacing( const std::vector<T>& collection ) {
        size_t minCount = std::min( this->m_collection.size(), collection.size() );

        for( size_t i = 0; i < minCount; i++ ) {
            if( this->m_collection[ i ] != collection[ i ] ) {
                std::vector<T> replacements;
                size_t startReplace = i;
                for( ; i < minCount; i++ ) {
                    if( this->m_collection[ i ] == collection[ i ] )
                        break;
                    replacements.push_back( collection[ i ] );
                }

                this->ReplaceRange( startReplace, replacements );
            }
        }
    }

    inline void NotifyObservers( std::shared_ptr<CollectionChangedEvent<T>> eventArgs ) {
        for( auto& observer: this->m_observers ) {
            observer->OnEvent( this, eventArgs );
        }
    }

    std::vector<T> m_collection;
    std::set<IObserver*> m_observers;
};

typedef ObservableCollection<QString> StringCollection;
typedef ObservableCollection<std::shared_ptr<QObject>> Collection;
