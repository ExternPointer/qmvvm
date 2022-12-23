#pragma once

#include <vector>

namespace TestHelpers {

template<typename T>
inline void ReplaceValues( std::vector<T>* target, const std::vector<T>& toReplace, size_t index ) {
    for( std::size_t i = 0; i < toReplace.size(); i++ ) {
        ( *target )[ index + i ] = toReplace[ i ];
    }
}

template<typename T>
inline void InsertValues( std::vector<T>* target, const std::vector<T>& toInsert, size_t index ) {
    for( std::size_t i = 0; i < toInsert.size(); i++ ) {
        target->insert( target->begin() + index + i, toInsert[ i ] );
    }
}

template<typename T>
inline std::vector<T> GetSubVector( const std::vector<T>& vector, size_t index, size_t count ) {
    return std::vector<T>( vector.begin() + index, vector.begin() + index + count );
}

}
