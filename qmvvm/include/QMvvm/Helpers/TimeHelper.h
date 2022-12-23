#pragma once

#include <cstdint>
#include <ctime>


namespace Helpers {

inline uint64_t GetTimestamp() {
    time_t ltime;
    time( &ltime );
    return static_cast<uint64_t>( ltime );
}

}
