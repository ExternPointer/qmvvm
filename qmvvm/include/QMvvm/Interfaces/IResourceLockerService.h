#pragma once

#include <cstdint>
#include <mutex>
#include <typeindex>


class IResourceLockerService {
public:
    virtual ~IResourceLockerService() = default;

    template<typename TResource>
    std::lock_guard<std::mutex> LockResource( uint64_t id ) {
        auto index = std::type_index( typeid( TResource ) );
        return this->LockResource( index, id );
    }

protected:
    virtual std::lock_guard<std::mutex> LockResource( std::type_index typeIndex, uint64_t id ) = 0;
};
