#pragma once

#include <QObject>
#include <memory>
#include <mutex>

#include "QMvvm/Classes/ListModel.h"


class ListModelHelper : public QObject {
    Q_OBJECT

public:
    ListModelHelper();

    Q_INVOKABLE ListModel* fromCollection( std::shared_ptr<Collection> collection );
    Q_INVOKABLE ListModel* fromStringCollection( std::shared_ptr<StringCollection> collection );

    Q_INVOKABLE QVariant toCollection( QVariantList variantList );
    Q_INVOKABLE QVariant toStringCollection( QVariantList variantList );
};
