#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "QMvvm/Interfaces/IResourceLockerService.h"


class ResourceLockerService : public IResourceLockerService {
public:
    ResourceLockerService();
    ~ResourceLockerService() override;

protected:
    std::lock_guard<std::mutex> LockResource( std::type_index typeIndex, uint64_t id ) override;

private:
    std::unordered_map<std::type_index, std::unordered_map<uint64_t, std::unique_ptr<std::mutex>>> m_mutexMap;
    std::mutex m_mutex;
};
