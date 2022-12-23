#include <QDebug>
#include <QQmlContext>

#include "../QmlHelpers/AsyncHelper.h"
#include "../QmlHelpers/ListModelHelper.h"
#include "QMvvm/Classes/ListModel.h"
#include "QMvvm/Classes/ProxyModel.h"
#include "QMvvm/Services/ApplicationService.h"


ApplicationService::ApplicationService( std::shared_ptr<CommandLineArgs> cmdArgs, std::shared_ptr<ListModelHelper> listModelHelper,
                                        std::shared_ptr<AsyncHelper> asyncHelper )
    : QObject( nullptr ) {
    this->m_application = std::make_shared<QGuiApplication>( cmdArgs->GetArgc(), cmdArgs->GetArgv() );

    this->m_listModelHelper = std::move( listModelHelper );
    this->m_asyncHelper = std::move( asyncHelper );

    this->m_translator = std::make_unique<QTranslator>();
    this->m_qmlEngine = std::make_unique<QQmlApplicationEngine>();
    qInfo() << "ApplicationService created";

    Q_INIT_RESOURCE( qmvvm_qml );
    this->RegisterTypes();
    this->ShowMainWindow();
}

ApplicationService::~ApplicationService() {
    qInfo() << "ApplicationService destroyed";
}

void ApplicationService::SetWindowTitle( const QString& title ) {
    this->m_qmlEngine->rootContext()->setContextProperty( "windowTitle", title );
}

void ApplicationService::SetLanguage( const QString& languageCode ) {
    qInfo() << "Setup translator: " << languageCode;
    if( !this->m_translator->isEmpty() ) {
        this->m_application->removeTranslator( this->m_translator.get() );
    }
    if( this->m_translator->load( QString( ":/i18n/IconChanger_%0.qm" ).arg( languageCode ) ) ) {
        this->m_application->installTranslator( this->m_translator.get() );
    } else {
        qWarning() << "Cant install translation";
    }
    this->m_qmlEngine->retranslate();
}

QString ApplicationService::GetLanguage() {
    return this->m_translator->language();
}

int ApplicationService::Run() {
    return this->m_application->exec();
}

std::shared_ptr<QGuiApplication> ApplicationService::GetApplication() {
    return this->m_application;
}

void ApplicationService::RegisterViewModel( std::shared_ptr<IViewModel> viewModel ) {
    auto index = std::type_index( typeid( *viewModel.get() ) );
    if( this->m_registeredViewModels.contains( index ) ) {
        qWarning() << "ViewModel redefinition";
    }
    this->m_registeredViewModels[ index ] = viewModel;
}

void ApplicationService::ShowView( std::type_index viewModel ) {
    if( !this->m_registeredViewModels.contains( viewModel ) ) {
        qCritical() << "ViewModel is not registered:" << viewModel.name();
        return;
    }
    this->ShowView( this->m_registeredViewModels[ viewModel ] );
}

void ApplicationService::ShowView( std::shared_ptr<IViewModel> viewModel ) {
    auto viewModelClassName = QString( viewModel->metaObject()->className() );
    QString view = viewModelClassName.remove( "ViewModel" );
    view[ 0 ] = view[ 0 ].toLower();
    this->ShowView( view, viewModel );
}

void ApplicationService::ShowView( const QString& viewName, std::shared_ptr<IViewModel> viewModel ) {
    qInfo() << "Swithcing to view: " << viewName;
    QString qmlFile = QString( "qrc:/Qml/Views/%0.qml" ).arg( viewName );
    viewModel->OnShowView( this->weak_from_this() );
    emit this->showViewSignal( qmlFile, QVariant::fromValue( viewModel.get() ) );
    this->m_currentViewModel = viewModel;
}

void ApplicationService::RegisterTypes() {
    qmlRegisterType<ProxyModel>( "QMvvm", 1, 0, "ProxyModel" );
    qmlRegisterSingletonInstance<ListModelHelper>( "QMvvm.Internal", 1, 0, "ListModelHelper", this->m_listModelHelper.get() );
    qmlRegisterSingletonInstance<AsyncHelper>( "QMvvm.Internal", 1, 0, "AsyncHelper", this->m_asyncHelper.get() );
    qmlRegisterSingletonInstance<ApplicationService>( "QMvvm", 1, 0, "ApplicationService", this );
}

void ApplicationService::ShowMainWindow() {
    qInfo() << "Creating main window...";
    this->m_qmlEngine->addImportPath( "qrc:/Qml" );
    const QUrl url( QStringLiteral( "qrc:/Qml/main.qml" ) );
    QObject::connect(
        this->m_qmlEngine.get(), &QQmlApplicationEngine::objectCreated, this->m_application.get(),
        [ url ]( QObject* obj, const QUrl& objUrl ) {
            if( !obj && url == objUrl ) {
                QCoreApplication::exit( -1 );
            }
        },
        Qt::QueuedConnection );
    this->m_qmlEngine->load( url );
}