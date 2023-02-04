#include "UIViewR.h"
#include "ui_UIViewR.h"

#include "UIMainWindow.h"
#include "UIViewL.h"

#include "ItemL.h"
#include "ItemR.h"
#include "utils.h"
#include "ContextMenu.h"
#include "FileDB.h"

#include "DriveCache.h"
#include "ActionFile.h"

//////////////////////////////////////////////////////////////////////////////////
class UIViewR::Impl : public Ui_UIViewR {
public:
	using self_t = UIViewR;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	std::unique_ptr<ContextMenu> contextMenuFile;
	std::unique_ptr<ContextMenu> contextMenuSymLink;

	QList<QWidget*> view;
	QHash<QString, ItemR*> cacheItemR;


	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

		view << treeWidget;
		view << treeWidget_2;
	}


	/////////////////////////////////////////
	void _start() {
		$::restoreState( treeWidget->header(), config.treeWidgetHeaderSize );
		$::restoreState( treeWidget_2->header(), config.treeWidget2_HeaderSize );

		createContextMenu();
		createContextMenuSymLink();

		treeWidget->sortByColumn( eSize, Qt::SortOrder::DescendingOrder );

		$TreeWidget::itemDoubleClicked( treeWidget, std::bind( &Impl::treeWidget_itemDoubleClicked, this ) );
		$TreeWidget::itemDoubleClicked( treeWidget_2, std::bind( &Impl::treeWidget_2_itemDoubleClicked, this ) );

		//tabWidget->setCurrentIndex( 0 );
		treeWidget_2->hide();

		// 左ビューの選択
		connect(
			qtWindow,
			&UIMainWindow::uiViewL_selectPath,
			std::bind( &Impl::selectPath, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::changeDrive,
			std::bind( &Impl::changeDrive, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::setBrowseMode,
			std::bind( &Impl::setBrowseMode, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::deleteFolder,
			self,
			std::bind( &Impl::deleteFolder, this, std::placeholders::_1 ) );
	}


	/////////////////////////////////////////
	void _closeWindow() {
		$::saveState( config.treeWidgetHeaderSize, treeWidget->header() );
		$::saveState( config.treeWidget2_HeaderSize, treeWidget_2->header() );
	}


	/////////////////////////////////////////
	void deleteFolder( const QString& fullPath ) {

		auto it = cacheItemR.find( fullPath );

		if( it == cacheItemR.constEnd() ) {
			return;
		}

		( *it )->setHidden( true );
	}


	/////////////////////////////////////////
	void setBrowseMode( int n ) {
		for( int i = 0; i < view.length(); i++ ) {
			view[ i ]->setVisible( i == n );
		}
	}


	/////////////////////////////////////////
	void changeDrive( QChar driveName ) {
		auto& dc = fileDB.driveCache( driveName );

		if( future.isRunning() ) {
			future.waitForFinished();
		}

		treeWidget_2->clear();

		future = QtConcurrent::run( [&]() {
			for( auto& p : dc.symbolicLink.keys() ) {
				auto* iyem = new ItemSymLink( treeWidget_2, p, dc.symbolicLink[ p ] );

				treeWidget_2->addTopLevelItem( iyem );
			}
		} );
		watcher.setFuture( future );
	}


	/////////////////////////////////////////
	/// ダブルクリック
	void treeWidget_itemDoubleClicked() {
		auto* item = treeWidget->currentItem<ItemR>();
		if( !item ) return;

		if( item->isFolder() ) {
			//emit self->itemDoubleClicked( item );
			qtWindow->uiViewL_selectPath( item->fullPath );
		}
		else {
			item->openFile();
		}
	}


	/////////////////////////////////////////
	/// ダブルクリック
	void treeWidget_2_itemDoubleClicked() {
		auto* item = treeWidget_2->currentItem<ItemSymLink>();
		if( !item ) return;

		emit qtWindow->uiViewL_selectPath( item->fullPath );
	}


	/////////////////////////////////////////
	void selectPath( QString fullPath ) {
		treeWidget->clear();
		cacheItemR.clear();
		//tabWidget->setCurrentIndex( 0 );

		//if( !item )return;
		//if( item->reference )return;
		HFileInfo fi( fullPath );
		if( fi.isLink() ) {
			auto* p = new ItemR( treeWidget, fi.linkTarget(), true );
			treeWidget->addTopLevelItem( p );
			return;
		}

		for( auto& pathName : fs::getDirectories( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
			auto* p = new ItemR( treeWidget, pathName );

			treeWidget->addTopLevelItem( p );
			cacheItemR.insert( pathName, p );
		}

		for( auto& pathName : fs::getFiles( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
			auto* p = new ItemR( treeWidget, pathName );

			treeWidget->addTopLevelItem( p );
			cacheItemR.insert( pathName, p );
		}
	}


	/////////////////////////////////////////
	void createContextMenu() {
		treeWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		$TreeWidget::customContextMenuRequested( treeWidget, [&]( const QPoint& pos ) {
			QModelIndex index = treeWidget->indexAt( pos );
			if( !index.isValid() ) return;

			auto* itemBase = reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() );

			auto* itemFileInfo = dynamic_cast<ItemFileInfo*>( itemBase );

			contextMenuFile = std::make_unique<ContextMenu>();

			if( itemFileInfo ) {
				if( itemFileInfo->isFile() ) {
					contextMenuFile->addAction<ActionOpenFile>( treeWidget );
				}
				contextMenuFile->addAction<ActionShowInExplorer>( treeWidget );
				contextMenuFile->addAction<ActionCopyFileName>( treeWidget );
#ifdef Q_OS_WIN
				contextMenuFile->addSeparator();
				auto lst = QStandardPaths::standardLocations( QStandardPaths::ApplicationsLocation );
				auto w = path::getDirectoryName( path::getDirectoryName( lst[ 0 ] ) );
				w += "/SendTo";
				for( auto& f : fs::getFiles( w, "*.lnk" ) ) {
					contextMenuFile->addAction<Action_LnkFile>( treeWidget, f );
				}
#endif
				contextMenuFile->addSeparator();
				contextMenuFile->addAction<ActionDelete>( treeWidget );
				contextMenuFile->addAction<ActionRename>( treeWidget );
				contextMenuFile->addSeparator();
				contextMenuFile->addAction<ActionShowProperty>( treeWidget );
			}

			contextMenuFile->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		} );
	}


	/////////////////////////////////////////
	void createContextMenuSymLink() {
		treeWidget_2->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		$TreeWidget::customContextMenuRequested( treeWidget_2, [&]( const QPoint& pos ) {
			QModelIndex index = treeWidget_2->indexAt( pos );
			if( !index.isValid() ) return;

			contextMenuSymLink = std::make_unique<ContextMenu>();

			auto* itemBase = reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() );

			auto* itemFileInfo = dynamic_cast<ItemFileInfo*>( itemBase );
			if( itemFileInfo ) {
				contextMenuSymLink->addAction<Action_ShowTargetPath>( treeWidget_2 );
				contextMenuSymLink->addSeparator();
				contextMenuSymLink->addAction<ActionShowProperty>( treeWidget_2 );
			}


			//auto* item = dynamic_cast<ItemL*>( reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() ) );
			//if( item ) {

			//	contextMenu->addAction<Action_Analize>( treeWidget );
			//	//( *contextMenu )->addAction<ActionShowInExplorer>( treeWidget );
			//	//( *contextMenu )->addAction<ActionCopyFileName>( treeWidget );


			//}
			contextMenuSymLink->exec( treeWidget_2->viewport()->mapToGlobal( pos ) );
		} );
	}

};


//////////////////////////////////////////////////////////////////////////////////
UIViewR::UIViewR( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, std::bind( &Impl::_start, impl.get() ) );
	connect( qtWindow, &UIMainWindow::signal_closeWindow, std::bind( &Impl::_closeWindow, impl.get() ) );
}


/////////////////////////////////////////
UIViewR::~UIViewR() {
}


/////////////////////////////////////////
//void UIViewR::setFolder( ItemL* itemL ) {
//	impl->setFolder( itemL );
//}
