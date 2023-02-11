#include "TreeViewSymLink.h"

#include "UIMainWindow.h"
#include "UIViewL.h"
#include "ContextMenu.h"
#include "ActionFile.h"
//#include "ItemL.h"
#include "ItemSymLink.h"
//#include "ItemR.h"
#include "FileDB.h"
#include "utils.h"

//////////////////////////////////////////////////////////////////////////////////
class TreeViewSymLink::Impl {
public:
	using self_t = TreeViewSymLink;
	self_t* self;

	std::unique_ptr<ContextMenu> contextMenu;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	QHash<QChar, QList<ItemSymLink*>> itemCache;

	QChar currentDrive;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

	}


	/////////////////////////////////////////
	void _start() {
		$::restoreState( self->header(), config.treeWidget2_HeaderSize );
		self->sortByColumn( 0, Qt::SortOrder::DescendingOrder );

		utils::createContextMenu( &contextMenu, self );

		$TreeWidget::itemDoubleClicked( self, std::bind( &Impl::itemDoubleClicked, this ) );

		connect(
			qtWindow,
			&UIMainWindow::signal_changeDrive,
			self,
			std::bind( &Impl::changeDrive, this, std::placeholders::_1 ) );

		connect(
			self,
			&TreeViewSymLink::signal_addTop,
			self,
			std::bind( &QTreeWidget::addTopLevelItem, self, std::placeholders::_1 ) );

		connect(
			self,
			&self_t::signal_setHidden,
			self,
			std::bind( &Impl::setHidden, this, std::placeholders::_1 ) );

		/// キャッシュデータの読み込み完了
		connect(
			&fileDB,
			&FileDB::signal_completeFileLoad,
			self,
			std::bind( &Impl::completeFileLoad, this ) );
	}


	/////////////////////////////////////////
	void _closeWindow() {
		$::saveState( config.treeWidget2_HeaderSize, self->header() );
	}


	/////////////////////////////////////////
	void setHidden( QTreeWidgetItem* item ) {
		item->setHidden( false );
	}


	/////////////////////////////////////////
	/// キャッシュデータの読み込み完了
	void completeFileLoad() {
		changeDrive( config.driveLetter );
	}


	/////////////////////////////////////////
	void changeDrive( QChar driveName ) {

		if( future.isRunning() ) {
			future.waitForFinished();
		}

		for( auto& k : itemCache.keys() ) {
			for( auto* p : itemCache[ k ] ) {
				p->setHidden( true );
			}
		}

		currentDrive = driveName;

		future = QtConcurrent::run( [&, driveName]() {
			auto& dc = fileDB.driveCache( driveName );
			if( dc.isEmpty() )return;

			auto it = itemCache.find( driveName );
			if( it == itemCache.constEnd() ) {
				itemCache.insert( driveName, QList<ItemSymLink*>() );
				it = itemCache.find( driveName );
				for( auto& p : dc.symbolicLink.keys() ) {
					if( !fs::isExistDirectory( p ) ) {
						qDebug() << p;
						continue;
					}
					auto* item = new ItemSymLink( self, p, dc.symbolicLink[ p ] );

					self->addTop( item );
					( *it ) << item;
				}
			}
			else {
				for( auto* p : *it ) {
					self->setHidden( p );
				}
			}
		} );
		watcher.setFuture( future );
	}


	/////////////////////////////////////////
	/// ダブルクリック
	void itemDoubleClicked() {
		auto* item = self->currentItem<ItemSymLink>();
		if( !item ) return;

		qtWindow->selectPath( item->fullPath );
	}

};


//////////////////////////////////////////////////////////////////////////////////
TreeViewSymLink::TreeViewSymLink( QWidget* parent /*= nullptr*/ ) :
	HTreeWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, std::bind( &Impl::_start, impl.get() ) );
	connect( qtWindow, &UIMainWindow::signal_closeWindow, std::bind( &Impl::_closeWindow, impl.get() ) );
}


/////////////////////////////////////////
void TreeViewSymLink::addTop( QTreeWidgetItem* item ) {
	emit signal_addTop( item );
}

/////////////////////////////////////////
void TreeViewSymLink::setHidden( QTreeWidgetItem* item ) {
	emit signal_setHidden( item );
}

