#include "DriveCache.h"
#include "FileDB.h"

#define MAKE_SIG(n1,n2,n3,n4)  n4 | (n3<<8) | (n2<<16)| (n1<<24)
static const quint32 FFDB = MAKE_SIG( 'F', 'F', 'D', 'B' );
static const quint32 VER = MAKE_SIG( 'V', '1', '0', '1' );

///////////////////////////////////////
inline
QDataStream& operator<<( QDataStream& out, SizeSet& hash ) {
	out << hash.folderFileSize << hash.fileSize;
	return out;
}


///////////////////////////////////////
inline
QDataStream& operator>>( QDataStream& in, SizeSet& hash ) {
	in >> hash.folderFileSize >> hash.fileSize;
	return in;
}


///////////////////////////////////////
inline
QDataStream& operator<<( QDataStream& out, DriveCache& hash ) {

	out << FFDB;
	out << VER;

	int keysXount = hash.folderFileSize.keys().length();
	out << keysXount;

	auto it = hash.folderFileSize.begin();
	auto ed = hash.folderFileSize.end();
	for( ; it != ed; it++ ) {
		out << it.key() << it.value();
	}

	int num2 = hash.symbolicLink.keys().length();
	out << num2;

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


///////////////////////////////////////
inline
QDataStream& operator>>( QDataStream& in, DriveCache& hash ) {

	quint32 sig;
	in >> sig;
	if( sig != FFDB ) {
		throw - 1;
	}
	quint32 ver;
	in >> ver;
	if( ver != VER ) {
		throw - 2;
	}

	int num1 = 0;
	in >> num1;

	QString cKey;
	SizeSet cMyCls;

	hash.folderFileSize.reserve( num1 * 2 );

	for( int i = 0; i < num1; i++ ) {
		in >> cKey >> cMyCls;
		if( !cKey.isEmpty() ) { // prohibits the last empty one
			hash.folderFileSize.insert( cKey, cMyCls );
		}
	}
	//qDebug() <<hash.folderFileSize.capacity();
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
	try {
		binarySerializer::read( filePath, [&dc]( QDataStream& in ) {
			in >> dc;
		} );
	}
	catch( int code ) {
		emit fileDB.signal_errorCache( path::getFileName( filePath ) );
	}
}


/////////////////////////////////////////
void DriveCache::write( DriveCache& dc, QChar driveName ) {
	binarySerializer::write( $$( "%1_drive.cache" ).arg( driveName ), [&dc]( auto& out ) {
		out << dc;
	} );
}


/////////////////////////////////////////
void DriveCache::set( const QString& fullPath, qint64 folderSize, qint64 fileSize ) {
	auto it = folderFileSize.find( fullPath );

	if( it == folderFileSize.constEnd() ) {
		folderFileSize.insert( fullPath, SizeSet{ folderSize,fileSize } );
	}
	else {
		( *it ) = SizeSet{ folderSize,fileSize };
	}
}


/////////////////////////////////////////
SizeSet DriveCache::get( const QString& fullPath ) {
	auto it = folderFileSize.find( fullPath );
	if( it == folderFileSize.constEnd() ) {
		return SizeSet{ 0 ,0 };
	}
	return ( *it );
}


/////////////////////////////////////////
void DriveCache::setSymbolicLink( const QString& fullPath, const QString& targetPath ) {
	qDebug() << $$( "in: %1 : %2" ).arg( fullPath ).arg( targetPath );
	auto it = symbolicLink.find( fullPath );
	if( it == symbolicLink.constEnd() ) {
		symbolicLink.insert( fullPath, targetPath );
	}
	else {
		( *it ) = targetPath;
	}
}

/////////////////////////////////////////
bool DriveCache::isEmpty() {
	return folderFileSize.size() == 0 && symbolicLink.size()==0;
}
