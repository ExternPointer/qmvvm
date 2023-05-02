#pragma once

#include <QJSValue>
#include <QObject>
#include <memory>


namespace Coroutines {
class ThreadPool;
};

class AsyncHelper : public QObject {
    Q_OBJECT

    inline static const int s_maxTasksCount = 100;

public:
    explicit AsyncHelper( std::shared_ptr<Coroutines::ThreadPool> threadPool );
    ~AsyncHelper() override;

    Q_INVOKABLE void runAsync( const QVariant& jsTask );

private:
    std::shared_ptr<Coroutines::ThreadPool> m_threadPool;
};
