import QtQuick
import QtQuick.Window
import QMvvm 1.0


Window {
    id: root
    minimumWidth: 1000
    minimumHeight: 600
    visible: true
    title: typeof windowTitle !== 'undefined' ? windowTitle : ""

    Loader {
        id: viewLoader
        focus: true
        anchors.fill: parent
    }

    Connections {
        target: ApplicationService
        function onShowViewSignal( qmlFile, viewModel ) {
            if ( viewModel ) {
                viewLoader.setSource( qmlFile, { "viewModel": viewModel } );
            } else {
                viewLoader.setSource( qmlFile );
                console.log( "View wasn't applied. Can't apply a view which value is null" )
            }
        }
    }
}
