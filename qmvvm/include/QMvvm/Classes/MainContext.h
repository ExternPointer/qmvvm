#pragma once

#include <QGuiApplication>
#include <QThread>
#include <QTimer>
#include <atomic>
#include <coroutine>
#include <Coroutines/Async.h>

#include "QMvvm/Interfaces/IApplicationService.h"


class MainContext {
public:
    class ScheduleOperation {
    public:
        ScheduleOperation( MainContext* mainContext ) noexcept
            : m_mainContext( mainContext ) {
        }

        bool await_ready() noexcept {
            return false;
        }
        void await_suspend( std::coroutine_handle<> awaitingCoroutine ) noexcept;
        void await_resume() noexcept {
        }

    private:
        friend class MainContext;

        MainContext* m_mainContext;
        std::coroutine_handle<> m_awaitingCoroutine;
    };

    MainContext( std::weak_ptr<IApplicationService> applicationService ) {
        this->m_applicationService = applicationService;
    }

    ~MainContext() {
    }

    [[nodiscard]] ScheduleOperation Schedule() noexcept {
        return ScheduleOperation { this };
    }

private:
    friend class ScheduleOperation;
    inline void ScheduleImpl( ScheduleOperation* operation ) noexcept {
        auto applicationService = this->m_applicationService.lock();
        if( !applicationService ) {
            operation->m_awaitingCoroutine.destroy();
            return;
        }
        if( QThread::currentThread() == applicationService->GetApplication()->thread() ) {
            operation->m_awaitingCoroutine.resume();
            return;
        }
        QTimer* timer = new QTimer();
        timer->moveToThread( applicationService->GetApplication()->thread() );
        timer->setSingleShot( true );
        QObject::connect( timer, &QTimer::timeout, [ = ]() { operation->m_awaitingCoroutine.resume(); } );
        QMetaObject::invokeMethod( timer, "start", Qt::QueuedConnection, Q_ARG( int, 0 ) );
    }

    std::weak_ptr<IApplicationService> m_applicationService;
};

inline void MainContext::ScheduleOperation::await_suspend( std::coroutine_handle<> awaitingCoroutine ) noexcept {
    this->m_awaitingCoroutine = awaitingCoroutine;
    this->m_mainContext->ScheduleImpl( this );
}