#pragma once

#include <QObject>
#include <memory>

class IApplicationService;


class IViewModel : public QObject {
public:
    explicit IViewModel( QObject* parent = nullptr )
        : QObject( parent ) {
    }
    virtual void OnShowView( std::weak_ptr<IApplicationService> applicationService ) = 0;
};
