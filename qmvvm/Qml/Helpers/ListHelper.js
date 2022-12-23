.import QMvvm.Internal 1.0 as QMvvmInternal

function fromCollection( source ) {
    if( Array.isArray( source ) ) {
        return source;
    }
    return QMvvmInternal.ListModelHelper.fromCollection( source );
}

function fromStringCollection( source ) {
    if( Array.isArray( source ) ) {
        return source;
    }
    return QMvvmInternal.ListModelHelper.fromStringCollection( source );
}

function toCollection( list ) {
    return QMvvmInternal.ListModelHelper.toCollection( list );
}

function toStringCollection( list ) {
    return QMvvmInternal.ListModelHelper.toStringCollection( list );
}
