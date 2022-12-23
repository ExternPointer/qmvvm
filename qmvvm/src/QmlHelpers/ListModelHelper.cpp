#include <QQmlEngine>
#include <vector>

#include "ListModelHelper.h"


ListModelHelper::ListModelHelper()
    : QObject( nullptr ) {
}

ListModel* ListModelHelper::fromCollection( std::shared_ptr<Collection> collection ) {
    auto connector = std::make_unique<CollectionConnectorTemplated<std::shared_ptr<QObject>>>( collection );
    auto listModel = new ListModel( std::move( connector ) );
    QQmlEngine::setObjectOwnership( listModel, QQmlEngine::JavaScriptOwnership );
    return listModel;
}

ListModel* ListModelHelper::fromStringCollection( std::shared_ptr<StringCollection> collection ) {
    auto connector = std::make_unique<CollectionConnectorTemplated<QString>>( collection );
    auto listModel = new ListModel( std::move( connector ) );
    QQmlEngine::setObjectOwnership( listModel, QQmlEngine::JavaScriptOwnership );
    return listModel;
}

QVariant ListModelHelper::toCollection( QVariantList variantList ) {
    auto list = std::vector<std::shared_ptr<QObject>>();
    for( int i = 0; i < variantList.count(); i++ ) {
        auto object = variantList[ i ].value<QObject*>();
        QQmlEngine::setObjectOwnership( object, QQmlEngine::CppOwnership );
        list.push_back( std::shared_ptr<QObject>( object ) );
    }
    auto collection = std::make_shared<Collection>( list );
    return QVariant::fromValue( collection );
}

QVariant ListModelHelper::toStringCollection( QVariantList variantList ) {
    auto list = std::vector<QString>();
    for( int i = 0; i < variantList.count(); i++ ) {
        auto string = variantList[ i ].value<QString>();
        list.push_back( string );
    }
    auto collection = std::make_shared<StringCollection>( list );
    return QVariant::fromValue( collection );
}
