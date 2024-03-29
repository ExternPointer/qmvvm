#include <QDebug>
#include <QVariant>
#include <utility>

#include <Coroutines/Async.h>

#include "AsyncHelper.h"


AsyncHelper::AsyncHelper( std::shared_ptr<ThreadPool> threadPool )
    : QObject( nullptr ) {
    this->m_threadPool = std::move( threadPool );
    qDebug() << "AsyncHelper created";
}

AsyncHelper::~AsyncHelper() {
    qDebug() << "AsyncHelper destroyed";
}

void AsyncHelper::runAsync( const QVariant& task ) {
    RunAsync( std::move( *( *(std::shared_ptr<Task<void>>*)task.constData() ) ), this->m_threadPool );
}
