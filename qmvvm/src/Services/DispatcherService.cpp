#include <QDebug>
#include <QGuiApplication>
#include <QTimer>

#include "QMvvm/Services/DispatcherService.h"


DispatcherService::DispatcherService( std::shared_ptr<IApplicationService> application ) {
    this->m_applicationService = application;
}

DispatcherService::~DispatcherService() {
    qInfo() << "DispatcherService destroyed";
}

void DispatcherService::DispatchToMainThread( std::function<void()> task ) {
    QTimer* timer = new QTimer();
    timer->moveToThread( this->m_applicationService->GetApplication()->thread() );
    timer->setSingleShot( true );
    QObject::connect( timer, &QTimer::timeout, [ = ]() {
        task();
        timer->deleteLater();
    } );
    QMetaObject::invokeMethod( timer, "start", Qt::QueuedConnection, Q_ARG( int, 0 ) );
}
