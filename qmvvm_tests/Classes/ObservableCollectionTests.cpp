#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <vector>

#include <QMvvm/Classes/ObservableCollection.h>

#include "TestHelpers/RelayObserver.h"
#include "TestHelpers/VectorHelper.h"

using namespace testing;
using namespace TestHelpers;

class ObservableCollectionFixture : public Test {
protected:
    static RelayObserver CreateObserver( ObservableCollection<QString>* collection, std::vector<QString>* initialVector ) {
        return RelayObserver( [ = ]( IObservable* source, const std::shared_ptr<BaseObserverEvent>& eventArgs ) {
            EXPECT_EQ( source, collection );
            auto args = std::static_pointer_cast<CollectionChangedEvent<QString>>( eventArgs );
            if( args->m_action == CollectionChangedAction::BeginReplace ) {
                EXPECT_EQ( collection->GetCollection(), *initialVector );
            } else if( args->m_action == CollectionChangedAction::Replace ) {
                EXPECT_EQ( args->m_oldItems, GetSubVector( *initialVector, args->m_startingIndex, args->m_oldItems.size() ) );
                ReplaceValues( initialVector, args->m_newItems, args->m_startingIndex );
                EXPECT_EQ( collection->GetCollection(), *initialVector );
            } else if( args->m_action == CollectionChangedAction::BeginAdd ) {
                EXPECT_EQ( collection->GetCollection(), *initialVector );
            } else if( args->m_action == CollectionChangedAction::Add ) {
                EXPECT_EQ( args->m_newItems, GetSubVector( collection->GetCollection(), args->m_startingIndex, args->m_newItems.size() ) );
                InsertValues( initialVector, args->m_newItems, args->m_startingIndex );
                EXPECT_EQ( collection->GetCollection(), *initialVector );
            } else if( args->m_action == CollectionChangedAction::BeginRemove ) {
                EXPECT_EQ( collection->GetCollection(), *initialVector );
            } else if( args->m_action == CollectionChangedAction::Remove ) {
                EXPECT_EQ( args->m_oldItems, GetSubVector( *initialVector, args->m_startingIndex, args->m_oldItems.size() ) );
                initialVector->erase( initialVector->begin() + (int)args->m_startingIndex,
                                      initialVector->begin() + (int)args->m_startingIndex + (int)args->m_oldItems.size() );
                EXPECT_EQ( collection->GetCollection(), *initialVector );
            } else {
                FAIL() << "Unknown action";
            }
        } );
    }
};
typedef ObservableCollectionFixture ObservableCollectionTests;


TEST_F( ObservableCollectionTests, DefaultContstructorCreatesEmptyCollection ) {
    // Arrange & Act
    ObservableCollection<QString> collection;

    // Assert
    EXPECT_EQ( collection.GetCollection().size(), 0ULL );
}

TEST_F( ObservableCollectionTests, ContructorFromArrayFillsCollection ) {
    // Arrange
    QString array[] = { "String1", "String2", "String3", "String4", "String5" };
    size_t arraySize = 5;

    // Act
    ObservableCollection<QString> collection( array, arraySize );

    // Assert
    ASSERT_THAT( collection.GetCollection(), ElementsAreArray( array ) );
}

TEST_F( ObservableCollectionTests, ContructorFromVectorFillsCollection ) {
    // Arrange
    std::vector<QString> vector { "String1", "String2", "String3", "String4", "String5" };

    // Act
    ObservableCollection<QString> collection( vector );

    // Assert
    EXPECT_EQ( collection.GetCollection(), vector );
}

TEST_F( ObservableCollectionTests, InsertRangeInFront ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toInsert { "6", "7", "8" };
    size_t index = 0;
    std::vector<QString> expectedResult { "6", "7", "8", "1", "2", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.InsertRange( index, toInsert );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, InsertRangeInMiddle ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toInsert { "6", "7", "8" };
    size_t index = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "8", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.InsertRange( index, toInsert );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, InsertRangeInBack ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toInsert { "6", "7", "8" };
    size_t index = 5;
    std::vector<QString> expectedResult { "1", "2", "3", "4", "5", "6", "7", "8" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.InsertRange( index, toInsert );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, InsertRangeThrowsExceptionCase1 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toInsert { "6", "7", "8" };
    size_t index = -1;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.InsertRange( index, toInsert ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, InsertRangeThrowsExceptionCase2 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toInsert { "6", "7", "8" };
    size_t index = 6;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.InsertRange( index, toInsert ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, InsertRangeNotifyObserver ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toInsert { "6", "7", "8" };
    size_t index = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "8", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.InsertRange( index, toInsert );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, RemoveRangeByCollection ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toRemove { "5", "2", "4", "1" };
    std::vector<QString> expectedResult { "3" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.RemoveRange( toRemove );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, RemoveRangeByCollectionNotifyObserver ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toRemove { "5", "2", "4", "1" };
    std::vector<QString> expectedResult { "3" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.RemoveRange( toRemove );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountInFront ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = 0;
    size_t count = 2;
    std::vector<QString> expectedResult { "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.RemoveRange( index, count );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountInMiddle ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = 2;
    size_t count = 2;
    std::vector<QString> expectedResult { "1", "2", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.RemoveRange( index, count );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountInBack ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = 3;
    size_t count = 2;
    std::vector<QString> expectedResult { "1", "2", "3" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.RemoveRange( index, count );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}


TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountThrowsExceptionCase1 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = -1;
    size_t count = 3;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.RemoveRange( index, count ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountThrowsExceptionCase2 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = 3;
    size_t count = 3;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.RemoveRange( index, count ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountThrowsExceptionCase3 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = 5;
    size_t count = 0;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.RemoveRange( index, count ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, RemoveRangeByIndexAndCountNotifyObserver ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    size_t index = 2;
    size_t count = 2;
    std::vector<QString> oldItems { "3", "4" };
    std::vector<QString> expectedResult { "1", "2", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.RemoveRange( index, count );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, RemoveAll ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.RemoveAll();

    // Assert
    EXPECT_EQ( collection.GetCollection().size(), 0ULL );
}

TEST_F( ObservableCollectionTests, RemoveAllInEmptyCollection ) {
    // Arrange
    ObservableCollection<QString> collection;

    RelayObserver observer( [ & ]( IObservable* source, const std::shared_ptr<BaseObserverEvent>& eventArgs ) {
        Q_UNUSED( source )
        Q_UNUSED( eventArgs )
        FAIL() << "RemoveAll on empty collection should do nothing";
    } );
    collection.AttachObserver( &observer );

    // Act
    collection.RemoveAll();

    // Assert
    EXPECT_EQ( collection.GetCollection().size(), 0ULL );
}

TEST_F( ObservableCollectionTests, RemoveAllNotifyObserver ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.RemoveAll();

    // Assert
    EXPECT_EQ( initialVector.size(), 0ULL );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionInFront ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 0;
    std::vector<QString> expectedResult { "6", "7", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionInMiddle ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionInBack ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 3;
    std::vector<QString> expectedResult { "1", "2", "3", "6", "7" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionThrowsExceptionCase1 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = -1;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.ReplaceRange( index, toReplace ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionThrowsExceptionCase2 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 4;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.ReplaceRange( index, toReplace ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionNotifyObserver ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.ReplaceRange( index, toReplace );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountCase1 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace {};
    int index = 2;
    int count = 2;
    std::vector<QString> expectedResult { "1", "2", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountCase2 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    int count = 0;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountCase3 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7", "8" };
    int index = 2;
    int count = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "8", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountCase4 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    int count = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "5" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountCase5 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    int count = 3;
    std::vector<QString> expectedResult { "1", "2", "6", "7" };
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountThrowsExceptionCase1 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 3;
    int count = 3;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.ReplaceRange( index, count, toReplace ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountThrowsExceptionCase2 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = -1;
    int count = 3;
    ObservableCollection<QString> collection( initialVector );

    // Act & Assert
    EXPECT_THROW( collection.ReplaceRange( index, count, toReplace ), std::out_of_range );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountNotifyObserverCase1 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace {};
    int index = 2;
    int count = 2;
    std::vector<QString> expectedResult { "1", "2", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountNotifyObserverCase2 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    int count = 0;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "3", "4", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountNotifyObserverCase3 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7", "8" };
    int index = 2;
    int count = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "8", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountNotifyObserverCase4 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    int count = 2;
    std::vector<QString> expectedResult { "1", "2", "6", "7", "5" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, ReplaceRangeByCollectionAndCountNotifyObserverCase5 ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5" };
    std::vector<QString> toReplace { "6", "7" };
    int index = 2;
    int count = 3;
    std::vector<QString> expectedResult { "1", "2", "6", "7" };
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.ReplaceRange( index, count, toReplace );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, TransformTo ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    std::vector<QString> transformTo { "200", "10", "20", "3", "40", "5", "60", "70", "8", "90", "100" };
    const std::vector<QString>& expectedResult = transformTo;
    ObservableCollection<QString> collection( initialVector );

    // Act
    collection.TransformTo( transformTo );

    // Assert
    EXPECT_EQ( collection.GetCollection(), expectedResult );
}

TEST_F( ObservableCollectionTests, TransformToNotifyObserver ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    std::vector<QString> transformTo { "200", "10", "20", "3", "40", "5", "60", "70", "8", "90", "100" };
    const std::vector<QString>& expectedResult = transformTo;
    ObservableCollection<QString> collection( initialVector );

    RelayObserver observer = ObservableCollectionFixture::CreateObserver( &collection, &initialVector );
    collection.AttachObserver( &observer );

    // Act
    collection.TransformTo( transformTo );

    // Assert
    EXPECT_EQ( initialVector, expectedResult );
}

TEST_F( ObservableCollectionTests, AtachDetachObserversComplexTest ) {
    // Arrange
    std::vector<QString> initialVector { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    std::vector<QString> transformTo { "200", "10", "20", "3", "40", "5", "60", "70", "8", "90", "100" };
    ObservableCollection<QString> collection( initialVector );

    bool isObserver1ReceivedEvents = false;
    bool isObserver2ReceivedEvents = false;
    bool isObserver3ReceivedEvents = false;

    RelayObserver observer1( [ & ]( IObservable* source, const std::shared_ptr<BaseObserverEvent>& eventArgs ) {
        Q_UNUSED( eventArgs )
        EXPECT_EQ( source, &collection );
        isObserver1ReceivedEvents = true;
    } );

    RelayObserver observer2( [ & ]( IObservable* source, const std::shared_ptr<BaseObserverEvent>& eventArgs ) {
        Q_UNUSED( eventArgs )
        EXPECT_EQ( source, &collection );
        isObserver2ReceivedEvents = true;
    } );

    RelayObserver observer3( [ & ]( IObservable* source, const std::shared_ptr<BaseObserverEvent>& eventArgs ) {
        Q_UNUSED( eventArgs )
        EXPECT_EQ( source, &collection );
        isObserver3ReceivedEvents = true;
    } );

    collection.AttachObserver( &observer1 );
    collection.AttachObserver( &observer2 );
    collection.AttachObserver( &observer3 );

    collection.DetachObserver( &observer2 );

    // Act
    collection.TransformTo( transformTo );

    // Assert
    EXPECT_TRUE( isObserver1ReceivedEvents );
    EXPECT_FALSE( isObserver2ReceivedEvents );
    EXPECT_TRUE( isObserver3ReceivedEvents );
}
