#pragma once

#include <QGuiApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include <memory>
#include <unordered_map>

#include "../../../src/QmlHelpers/AsyncHelper.h"
#include "../../../src/QmlHelpers/ListModelHelper.h"
#include "QMvvm/Classes/CommandLineArgs.h"
#include "QMvvm/Interfaces/IApplicationService.h"
#include "QMvvm/Interfaces/IViewModel.h"


class ApplicationService : public QObject, public IApplicationService, public std::enable_shared_from_this<ApplicationService> {
    Q_OBJECT

public:
    ApplicationService( std::shared_ptr<CommandLineArgs> cmdArgs, std::shared_ptr<ListModelHelper> listModelHelper, std::shared_ptr<AsyncHelper> asyncHelper );
    ~ApplicationService() override;

    void SetWindowTitle( const QString& title ) override;
    void SetLanguage( const QString& languageCode ) override;
    QString GetLanguage() override;
    int Run() override;
    std::shared_ptr<QGuiApplication> GetApplication() override;

    void RegisterViewModel( std::shared_ptr<IViewModel> viewModel ) override;

signals:
    void showViewSignal( QString qmlFile, QVariant viewModel );

protected:
    void ShowView( std::type_index viewModel ) override;

private:
    void ShowView( std::shared_ptr<IViewModel> viewModel );
    void ShowView( const QString& viewName, std::shared_ptr<IViewModel> viewModel );

    void RegisterTypes();
    void ShowMainWindow();

    std::shared_ptr<QGuiApplication> m_application;
    std::shared_ptr<IViewModel> m_currentViewModel;
    std::unordered_map<std::type_index, std::shared_ptr<IViewModel>> m_registeredViewModels;
    std::shared_ptr<ListModelHelper> m_listModelHelper;
    std::unique_ptr<QTranslator> m_translator;
    std::unique_ptr<QQmlApplicationEngine> m_qmlEngine;
    std::shared_ptr<AsyncHelper> m_asyncHelper;
};
