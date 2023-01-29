#include "FileDB.h"
#include "UIMainWindow.h"

#include "UIStatusBar.h"

FileDB fileDB;

struct Folder {
	QString fullPath;
	qint64 folderSize;

	/////////////////////////////////////////
	Folder( const QString& _fullPath ) :
		fullPath( _fullPath ),
		folderSize( -1 ) {

	}

	/////////////////////////////////////////
	void calcSize( QFutureWatcher<void>& watcher ) {
		QList<Folder*> lst;

		if( fullPath == "O:/Asset Store-5.x" ) {
			qDebug() << "";
		}

		fs::enumerateDirectories( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden, [&]( const QString& pathName ) {
			watcher.progressTextChanged( pathName );

			QFileInfo finfo( pathName );
			bool b1 = finfo.isSymLink();
			bool b2 = finfo.isSymbolicLink();
			//bool b3 = ;
			bool b4 = finfo.isJunction();

			if( finfo.isShortcut() ) {
				return true;
			}
			if( b1 || b2 || b4 ) {
				fileDB.setSymbolicLink( pathName, finfo.symLinkTarget() );
				return true;
			}
			auto* p = new Folder( pathName );

			p->calcSize( watcher );
			lst << p;
			return true;
		} );

		if( fullPath == "O:/Asset Store-5.x" ) {
			qDebug() << "";
		}

		folderSize = 0;

		for( auto& p : lst ) {
			folderSize += p->folderSize;
		}

		fs::enumerateFiles( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden, [&]( const QString& pathName ) {
			folderSize += QFileInfo( pathName ).size();
			return true;
		} );

		fileDB.setSize( fullPath, folderSize );
	}
};


//////////////////////////////////////////////////////////////////////////////////
class FileDB::Impl {
public:
	using self_t = FileDB;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	QFuture<void> futureInit;
	QFutureWatcher<void> watcherInit;

	QHash<QChar, DriveCache> sizeMap;

	//FileHash calcSize;

	QString analizeDriveName;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		/////////////////////////////////////////
		connect( &watcher, &QFutureWatcher<void>::progressTextChanged, self, std::bind( &Impl::watcher_progressTextChanged, this, std::placeholders::_1 ) );

		/////////////////////////////////////////
		connect( &watcher, &QFutureWatcher<void>::finished, self, std::bind( &Impl::watcher_finished, this ) );

		/////////////////////////////////////////
		connect( &watcherInit, &QFutureWatcher<void>::finished, self, std::bind( &Impl::watcherInit_finished, this ) );

		futureInit = QtConcurrent::run( [&]() {
			for( auto& filePath : fs::getFiles( environment::currentDirectory(), "*.cache", SearchOption::TopDirectoryOnly ) ) {
				QChar dr = path::getFileName( filePath )[ 0 ];

				DriveCache tmp;
				DriveCache::read( tmp, filePath );
				sizeMap[ dr ] = std::move( tmp );
			}
		} );
		watcherInit.setFuture( futureInit );
	}


	/////////////////////////////////////////
	void watcherInit_finished() {
		UIStatusBar::info( u8"キャッシュデータを読み込みました" );
	}


	/////////////////////////////////////////
	void watcher_progressTextChanged( const QString& progressText ) {
		UIStatusBar::info( progressText );
	}


	/////////////////////////////////////////
	void watcher_finished() {
		DriveCache::write( sizeMap[ analizeDriveName[ 0 ] ], analizeDriveName[ 0 ] );

		emit self->completeAnalize();
	}


	/////////////////////////////////////////
	DriveCache& driveCache( const QString& fullPath ) {
		return driveCache( fullPath[ 0 ] );
	}


	/////////////////////////////////////////
	DriveCache& driveCache( QChar drive ) {
		auto it = sizeMap.find( drive );
		if( it == sizeMap.constEnd() ) {
			sizeMap.insert( drive, DriveCache() );
			it = sizeMap.find( drive );
		}
		return ( *it );
	}


	/////////////////////////////////////////
	//void set( const QFileInfo& finfo ) {
	//	set( finfo.filePath(), finfo.size() );
	//}


	/////////////////////////////////////////
	void set( const QString& fullPath, qint64 size ) {
		driveCache( fullPath ).set( fullPath, size );
	}


	/////////////////////////////////////////
	void analize( const QString& driveName ) {
		qtWindow->statusBar()->info( u8"analize" );

		analizeDriveName = driveName;
		auto dname = driveName + ":/";

		future = QtConcurrent::run( [this, dname]() {
			auto f = Folder( dname );
			//calcSize.clear();
			f.calcSize( watcher );
		} );
		watcher.setFuture( future );

		emit self->startAnalize();
	}


	/////////////////////////////////////////
	void save() {
		for( auto& k : sizeMap.keys() ) {
			DriveCache::write( sizeMap[ k ], k );
		}
	}


	/////////////////////////////////////////
	void setSymbolicLink( const QString& fullPath, const QString& targetPath ) {
		//impl->setSymbolicLink( fullPath, targetPath );
		driveCache( fullPath ).setSymbolicLink( fullPath, targetPath );
	}


};



//////////////////////////////////////////////////////////////////////////////////
FileDB::FileDB() :
	impl( new Impl( this ) ) {

}


/////////////////////////////////////////
//void FileDB::set( const QFileInfo& finfo ) {
//	impl->set( finfo );
//}

/////////////////////////////////////////
void FileDB::setSize( const QString& fullPath, qint64 size ) {
	impl->set( fullPath, size );
}

/////////////////////////////////////////
qint64 FileDB::get( const QString& fullPath ) {
	return impl->driveCache( fullPath ).get( fullPath );
}


/////////////////////////////////////////
void FileDB::analize( const QString& driveName ) {
	impl->analize( driveName );
}


/////////////////////////////////////////
void FileDB::save() {
	impl->save();
}


/////////////////////////////////////////
void FileDB::setSymbolicLink( const QString& fullPath, const QString& targetPath ) {
	impl->setSymbolicLink( fullPath, targetPath );
}


/////////////////////////////////////////
DriveCache& FileDB::driveCache( QChar drive ) {
	return impl->driveCache( drive );
}


/////////////////////////////////////////
QStringList FileDB::symbolicLinkSource( const QString& fullPath ) {
	auto& dr = impl->driveCache( fullPath[0] );
	QStringList result;
	for( auto& f : dr.symbolicLink.keys() ) {
		if( dr.symbolicLink[ f ] == fullPath ) {
			result << f;
		}
	}
	return result;
}
