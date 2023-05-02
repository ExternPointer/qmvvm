#pragma once

#include <QObject>
#include <memory>


#define PROPERTY_WITH_CALLBACK( type, name, getter, setter, getterCallback )                                                                                   \
private:                                                                                                                                                       \
    Q_PROPERTY( type name READ getter WRITE setter NOTIFY name##Changed )                                                                                      \
public:                                                                                                                                                        \
    inline const type& getter() {                                                                                                                              \
        getterCallback();                                                                                                                                      \
        return this->m_##name;                                                                                                                                 \
    }                                                                                                                                                          \
    inline void setter( const type& name ) {                                                                                                                   \
        if( this->m_##name != name ) {                                                                                                                         \
            this->m_##name = name;                                                                                                                             \
            emit this->name##Changed();                                                                                                                        \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
Q_SIGNALS:                                                                                                                                                     \
    void name##Changed();                                                                                                                                      \
                                                                                                                                                               \
private:                                                                                                                                                       \
    type m_##name;

#define PROPERTY( type, name, getter, setter ) PROPERTY_WITH_CALLBACK( type, name, getter, setter, void )

#define SHARED_PROPERTY( type, name, getter, setter )                                                                                                          \
private:                                                                                                                                                       \
    Q_PROPERTY( type* name READ getter NOTIFY name##Changed )                                                                                                  \
public:                                                                                                                                                        \
    inline const std::shared_ptr<type>& getter##Shared() {                                                                                                     \
        return this->m_##name;                                                                                                                                 \
    }                                                                                                                                                          \
    inline type* getter() const {                                                                                                                              \
        return this->m_##name.get();                                                                                                                           \
    }                                                                                                                                                          \
    inline void setter( const std::shared_ptr<type>& name ) {                                                                                                  \
        if( this->m_##name != name ) {                                                                                                                         \
            this->m_##name = name;                                                                                                                             \
            emit this->name##Changed();                                                                                                                        \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
Q_SIGNALS:                                                                                                                                                     \
    void name##Changed();                                                                                                                                      \
                                                                                                                                                               \
private:                                                                                                                                                       \
    std::shared_ptr<type> m_##name;


#define UNIQUE_PROPERTY( type, name, getter, setter )                                                                                                          \
private:                                                                                                                                                       \
    Q_PROPERTY( type* name READ getter NOTIFY name##Changed )                                                                                                  \
public:                                                                                                                                                        \
    inline const std::unique_ptr<type>& getter##Unique() {                                                                                                     \
        return this->m_##name;                                                                                                                                 \
    }                                                                                                                                                          \
    inline type* getter() const {                                                                                                                              \
        return this->m_##name.get();                                                                                                                           \
    }                                                                                                                                                          \
    inline void setter( std::unique_ptr<type> name ) {                                                                                                         \
        this->m_##name = std::move( name );                                                                                                                    \
        emit this->name##Changed();                                                                                                                            \
    }                                                                                                                                                          \
Q_SIGNALS:                                                                                                                                                     \
    void name##Changed();                                                                                                                                      \
                                                                                                                                                               \
private:                                                                                                                                                       \
    std::unique_ptr<type> m_##name;

#define Q_INVOKABLE_ASYNC_2ARGS( qmlMethod, method, type1, name1, type2, name2 )                                                                               \
    inline Q_INVOKABLE std::shared_ptr<Task<void>> qmlMethod( const type1& name1, const type2& name2 ) {                                                       \
        auto task = this->method( name1, name2 );                                                                                                              \
        return std::make_shared<Task<void>>( std::move( task ) );                                                                                              \
    }                                                                                                                                                          \
    Task<void> method( type1 name1, type2 name2 );
