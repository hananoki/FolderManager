#include "FileDB.h"
#include "UIMainWindow.h"

#include "UIStatusBar.h"
#include "UIMainWindow.h"

FileDB fileDB;

//////////////////////////////////////////////////////////////////////////////////
struct Folder {
	QString fullPath;
	qint64 folderFileSize;

	/////////////////////////////////////////
	Folder( const QString& _fullPath ) :
		fullPath( _fullPath ),
		folderFileSize( -1 ) {

	}


	/////////////////////////////////////////
	void calcSize( QFutureWatcher<void>& watcher ) {
		QList<Folder*> lst;

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
			if( b4 ) {
				fileDB.setSymbolicLink( pathName, $::junctionTarget( pathName ) );
				return true;
			}
			if( b1 || b2 ) {
				fileDB.setSymbolicLink( pathName, finfo.symLinkTarget() );
				return true;
			}
			auto* p = new Folder( pathName );

			p->calcSize( watcher );
			lst << p;
			return true;
		} );

		folderFileSize = 0;

		for( auto& p : lst ) {
			folderFileSize += p->folderFileSize;
		}

		qint64 fileSize = 0;
		fs::enumerateFiles( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden, [&]( const QString& pathName ) {
			auto sz = QFileInfo( pathName ).size();;
			fileSize += sz;
			folderFileSize += sz;
			return true;
		} );

		fileDB.setSize( fullPath, folderFileSize, fileSize );
	}
};


//////////////////////////////////////////////////////////////////////////////////
class FileDB::Impl {
public:
	using self_t = FileDB;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;
	QFuture<void> future2;
	QFutureWatcher<void> watcher2;

	QFuture<void> futureInit;
	QFutureWatcher<void> watcherInit;

	QHash<QChar, DriveCache> sizeMap;


	QChar analizeDriveName; /// 保存名に使用

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		/////////////////////////////////////////
		connect( &watcher, &QFutureWatcher<void>::progressTextChanged, self, std::bind( &Impl::watcher_progressTextChanged, this, std::placeholders::_1 ) );
		connect( &watcher2, &QFutureWatcher<void>::progressTextChanged, self, std::bind( &Impl::watcher_progressTextChanged, this, std::placeholders::_1 ) );

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
		emit self->completeFileLoad();
	}


	/////////////////////////////////////////
	void watcher_progressTextChanged( const QString& progressText ) {
		UIStatusBar::info( progressText );
	}


	/////////////////////////////////////////
	void watcher_finished() {
		DriveCache::write( sizeMap[ analizeDriveName ], analizeDriveName );

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
	void driveCache( std::function<void( DriveCache& )> cb ) {
		for( auto& v : sizeMap.keys() ) {
			cb( sizeMap[ v ] );
		}
	}

	/////////////////////////////////////////
	//void set( const QFileInfo& finfo ) {
	//	set( finfo.filePath(), finfo.folderSize() );
	//}


	/////////////////////////////////////////
	void set( const QString& fullPath, qint64 folderSize, qint64 fileSize ) {
		driveCache( fullPath ).set( fullPath, folderSize, fileSize );
	}


	/////////////////////////////////////////
	void analize( const QString& fullPath ) {
		qtWindow->statusBar()->info( u8"analize" );

		analizeDriveName = fullPath[ 0 ];

		QFileInfo fi( fullPath );

		// ドライブのルート全体更新
		if( fi.isRoot() ) {

			future = QtConcurrent::run( [this, fullPath]() {
				auto f = Folder( fullPath );
				//calcSize.clear();
				f.calcSize( watcher );
			} );
			watcher.setFuture( future );

			emit self->startAnalize();
		}
		else {
			future = QtConcurrent::run( [this, fullPath]() {
				auto f = Folder( fullPath );
				//calcSize.clear();
				f.calcSize( watcher2 );

				auto paths = fullPath.split( "/" );
				paths.takeLast();
				while( !paths.isEmpty() ) {
					auto dname = paths.join( "/" );

					qint64 size = 0;
					auto ds = fs::getDirectories( dname, "*" );
					for( auto& dn : ds ) {
						size += fileDB.get( dn );
					}
					SizeSet ss = fileDB.getSizeSet( dname );
					size += ss.fileSize;
					fileDB.setSize( dname, size, ss.fileSize );
					paths.takeLast();
				}
			} );
			watcher.setFuture( future );
			//future2.waitForFinished();

			emit self->startAnalize();
		}
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
FileDB::FileDB() {

}


//////////////////////////////////////////////////////////////////////////////////
void FileDB::init() {
	impl = std::make_unique<Impl>( this );
}


/////////////////////////////////////////
void FileDB::setSize( const QString& fullPath, qint64 folderSize, qint64 fileSize ) {
	impl->set( fullPath, folderSize, fileSize );
}


/////////////////////////////////////////
qint64 FileDB::get( const QString& fullPath ) {
	return getSizeSet( fullPath ).folderFileSize;
}

/////////////////////////////////////////
SizeSet FileDB::getSizeSet( const QString& fullPath ) {
	return impl->driveCache( fullPath ).get( fullPath );
}

/////////////////////////////////////////
void FileDB::analize( const QString& fullPath ) {
	impl->analize( fullPath );
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
	QStringList result;
	impl->driveCache( [&result, fullPath]( DriveCache& dr ) {
		for( auto& f : dr.symbolicLink.keys() ) {
			if( dr.symbolicLink[ f ] == fullPath ) {
				result << f;
			}
		}
	} );

	//auto& dr = impl->driveCache( fullPath[0] );


	return result;
}
