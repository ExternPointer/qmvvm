#pragma once

#include <QDebug>
#include <QGuiApplication>
#include <QString>
#include <memory>
#include <typeindex>


class IViewModel;

class IApplicationService {
public:
    virtual ~IApplicationService() = default;

    virtual void SetWindowTitle( const QString& title ) = 0;
    virtual void SetLanguage( const QString& languageCode ) = 0;
    virtual QString GetLanguage() = 0;
    virtual int Run() = 0;
    virtual std::shared_ptr<QGuiApplication> GetApplication() = 0;

    virtual void RegisterViewModel( std::shared_ptr<IViewModel> viewModel ) = 0;
    template<typename TViewModel>
    void ShowView() {
        auto index = std::type_index( typeid( TViewModel ) );
        this->ShowView( index );
    }

protected:
    virtual void ShowView( std::type_index viewModel ) = 0;
};
