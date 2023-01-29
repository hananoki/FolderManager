#include "DriveCache.h"

inline
QDataStream& operator<<( QDataStream& out, DriveCache& hash ) {
	//QHashIterator<QString, qint64 > i( hash );
	//
	//while( i.hasNext() ) {
	//	i.next();
	//	auto cKey = i.key();
	//	out << cKey << i.value();
	//}
	out << hash.folderSize.keys().length();

	auto it = hash.folderSize.begin();
	auto ed = hash.folderSize.end();
	for( ; it != ed; it++ ) {
		out << it.key() << it.value();
	}

	out << hash.symbolicLink.keys().length();

	qDebug() << "----------";
	auto it2 = hash.symbolicLink.begin();
	auto ed2 = hash.symbolicLink.end();
	for( ; it2 != ed2; it2++ ) {
		qDebug() << it2.key();
		qDebug() << it2.value();
		out << it2.key() << it2.value();
	}
	return out;
}

inline
QDataStream& operator>>( QDataStream& in, DriveCache& hash ) {
	//hash.clear(); // clear possible items
	int num1 = 0;
	in >> num1;

	QString cKey;
	qint64 cMyCls;

	for( int i = 0; i < num1; i++ ) {
		in >> cKey >> cMyCls;
		if( !cKey.isEmpty() ) { // prohibits the last empty one
			hash.folderSize.insert( cKey, cMyCls );
		}
	}
	//while( !in.status() ) { // while not finished or corrupted
	//	
	//}

	int num2 = 0;
	in >> num2;

	QString sKey;
	QString sValue;

	for( int i = 0; i < num2; i++ ) {
		in >> sKey >> sValue;
		hash.symbolicLink.insert( sKey, sValue );
	}

	return in;
}


/////////////////////////////////////////
void DriveCache::read( DriveCache& dc, const QString& filePath ) {
	//FileHash fh;
	binarySerializer::read( filePath, [&dc]( QDataStream& in ) {
		in >> dc;
	} );
	//folderSize = std::move( fh );
}


/////////////////////////////////////////
void DriveCache::write( DriveCache& dc, QChar driveName ) {
	binarySerializer::write( $$( "%1_drive.cache" ).arg( driveName ), [&dc]( auto& out ) {
		out << dc;
	} );
}


/////////////////////////////////////////
void DriveCache::set( const QString& fullPath, qint64 size ) {
	auto it = folderSize.find( fullPath );

	if( it == folderSize.constEnd() ) {
		folderSize.insert( fullPath, size );
	}
	else {
		( *it ) = size;
	}
}


/////////////////////////////////////////
qint64 DriveCache::get( const QString& fullPath ) {
	auto it = folderSize.find( fullPath );
	if( it == folderSize.constEnd() ) {
		return 0;
	}
	return ( *it );
}


/////////////////////////////////////////
void DriveCache::setSymbolicLink( const QString& fullPath, const QString& targetPath ) {
	qDebug() << $$("in: %1 : %2").arg( fullPath ).arg( targetPath );
	auto it = symbolicLink.find( fullPath );
	if( it == symbolicLink.constEnd() ) {
		symbolicLink.insert( fullPath, targetPath );
	}
	else {
		( *it ) = targetPath;
	}
}
