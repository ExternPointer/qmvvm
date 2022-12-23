#include <QDebug>

#include "QMvvm/Services/ResourceLockerService.h"


ResourceLockerService::ResourceLockerService() {
    qInfo() << "ResourceLockerService created";
}

ResourceLockerService::~ResourceLockerService() {
    qInfo() << "ResourceLockerService destroyed";
}

std::lock_guard<std::mutex> ResourceLockerService::LockResource( std::type_index typeIndex, uint64_t id ) {
    std::mutex* mutex;
    {
        std::lock_guard locker( this->m_mutex );
        if( !this->m_mutexMap.contains( typeIndex ) ) {
            this->m_mutexMap[ typeIndex ] = std::unordered_map<uint64_t, std::unique_ptr<std::mutex>>();
        }
        auto& scope = this->m_mutexMap[ typeIndex ];
        if( !scope.contains( id ) ) {
            scope[ id ] = std::make_unique<std::mutex>();
        }
        mutex = scope[ id ].get();
    }
    return std::lock_guard( *mutex );
}
