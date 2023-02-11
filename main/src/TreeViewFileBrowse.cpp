#include "TreeViewFileBrowse.h"

#include "UIMainWindow.h"
#include "UIViewL.h"

#include "ContextMenu.h"
#include "ActionFile.h"
#include "ItemL.h"
#include "ItemR.h"
#include "utils.h"
#include "FileDB.h"

//////////////////////////////////////////////////////////////////////////////////
class TreeViewFileBrowse::Impl {
public:
	using self_t = TreeViewFileBrowse;
	self_t* self;

	std::unique_ptr<ContextMenu> contextMenuFile;

	QHash<QString, ItemR*> cacheFileFolderItems;
	QHash<QString, ItemR*> cacheLinkItems;
	//QList< ItemR*> showList;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	QString currentSelectPath;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		/////////////////////////////////////////
		//connect( &watcher, &QFutureWatcher<void>::finished, self, std::bind( &Impl::watcher_finished, this ) );
	}


	/////////////////////////////////////////
	//void watcher_finished() {
	//	self->repaint();
	//}


	/////////////////////////////////////////
	void _start() {
		$::restoreState( self->header(), config.treeWidgetHeaderSize );

		utils::createContextMenu( &contextMenuFile, self );

		self->sortByColumn( eSize, Qt::SortOrder::DescendingOrder );
		self->setEditTriggers( QTreeWidget::EditTrigger::SelectedClicked | QTreeWidget::EditTrigger::EditKeyPressed );

		$TreeWidget::itemDoubleClicked( self, std::bind( &Impl::treeWidget_itemDoubleClicked, this ) );

		/// アイテムのテキスト編集した時
		$TreeWidget::itemChanged( self, std::bind( &Impl::treeWidget_itemChanged, this, std::placeholders::_1, std::placeholders::_2 ) );

		// 左ビューの選択
		connect(
			qtWindow,
			&UIMainWindow::signal_selectPath,
			std::bind( &Impl::selectPath, this, std::placeholders::_1 ) );

		connect(
			self,
			&self_t::signal_addTop,
			self,
			std::bind( &QTreeWidget::addTopLevelItem, self, std::placeholders::_1 )
		);

		connect(
			self,
			&self_t::setHidden,
			self,
			[&]( QTreeWidgetItem* item ) {
			item->setHidden( false );
			//item->treeWidget
			//tree
		}
		);

		connect(
			qtWindow,
			&UIMainWindow::deleteItem,
			self,
			std::bind( &Impl::deleteItem, this, std::placeholders::_1 ) );

		connect( &fileDB, &FileDB::signal_completeFileLoad, std::bind( &Impl::completeFileLoad, this ) );

		//self->onMousePressEvent = [&]( QMouseEvent* e ) {

		//	QModelIndex index = self->indexAt( e->pos() );
		//	if( !index.isValid() ) return false;

		//	auto* item = reinterpret_cast<ItemFileInfo*>( index.internalPointer() );

		//	//auto* item = self->currentItem<ItemFileInfo>();
		//	if( item ) {
		//		if( e->button() == Qt::LeftButton ) {
		//			//$::showInExplorer( item->folderPath );
		//			auto* mimeData = new QMimeData;
		//			QList<QUrl> urls;
		//			urls << QUrl::fromLocalFile( item->fullPath );
		//			mimeData->setUrls( urls );
		//			//mimeData->setText( item->folderPath );
		//			auto* drag = new QDrag( self );
		//			drag->setMimeData( mimeData );
		//			drag->exec( Qt::CopyAction );
		//			qDebug() << item->fullPath;
		//		}
		//		//return true;
		//	}
		//	return false;
		//};
	}


	/////////////////////////////////////////
	void _closeWindow() {
		$::saveState( config.treeWidgetHeaderSize, self->header() );
		//$::saveState( config.treeWidget2_HeaderSize, treeWidget_2->header() );
	}


	/////////////////////////////////////////
	void deleteItem( const QString& fullPath ) {
		auto it = cacheFileFolderItems.find( fullPath );
		if( it == cacheFileFolderItems.constEnd() ) return;
		( *it )->setHidden( true );
	}


	/////////////////////////////////////////
	/// 新しいパスに移動
	void selectPath( const QString& fullPath ) {
		if( currentSelectPath == fullPath )return;
		currentSelectPath = fullPath;

		if( future.isRunning() ) {
			future.waitForFinished();
		}

		for( auto* v : cacheFileFolderItems.values() ) {
			v->setHidden( true );
		}
		for( auto* v : cacheLinkItems.values() ) {
			v->setHidden( true );
		}

		//showList.clear();

		HFileInfo fi( fullPath );

		if( fi.isLink() ) {
			auto it = cacheLinkItems.find( fi.linkTarget() );
			if( it == cacheLinkItems.constEnd() ) {
				auto* p = new ItemR( self, fi.linkTarget(), true );
				cacheLinkItems.insert( fi.linkTarget(), p );
				self->addTopLevelItem( p );
			}
			else {
				( *it )->setHidden( false );
			}
			return;
		}

		future = QtConcurrent::run( [&, fullPath]() {
			for( auto& dname : fs::getDirectories( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
				auto it = cacheFileFolderItems.find( dname );
				if( it == cacheFileFolderItems.constEnd() ) {
					auto* p = new ItemR( self, dname );

					self->addTop( p );
					cacheFileFolderItems.insert( dname, p );
					//showList << p;
				}
				else {
					emit self->setHidden( ( *it ) );
				}
			}

			for( auto& fname : fs::getFiles( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
				auto it = cacheFileFolderItems.find( fname );
				if( it == cacheFileFolderItems.constEnd() ) {
					auto* p = new ItemR( self, fname );

					self->addTop( p );
					cacheFileFolderItems.insert( fname, p );
					//showList << p;
				}
				else {
					emit self->setHidden( ( *it ) );
				}
			}
		} );
		watcher.setFuture( future );
	}


	/////////////////////////////////////////
	void treeWidget_itemChanged( QTreeWidgetItem* item, int column ) {
		if( column != 0 ) return;

		auto* it = dynamic_cast<ItemR*>( item );
		if( !it ) {
			UIStatusBar::warning( u8"アイテムの型が違う" );
			return;
		}

		if( !it->rename() ) return;

		cacheFileFolderItems.insert( it->fullPath, it );
	}


	/////////////////////////////////////////
	/// ダブルクリック
	void treeWidget_itemDoubleClicked() {
		auto* item = self->currentItem<ItemR>();
		if( !item ) return;

		if( item->isFolder() ) {
			qtWindow->selectPath( item->fullPath );
		}
		else {
			item->openFile();
		}
	}


	/////////////////////////////////////////
	void completeFileLoad() {
		for( auto& item : self->topLevelItems<ItemR>() ) {
			item->updateSize();
		}
		self->sortByColumn( eSize, Qt::SortOrder::DescendingOrder );
	}

};


//////////////////////////////////////////////////////////////////////////////////
TreeViewFileBrowse::TreeViewFileBrowse( QWidget* parent /*= nullptr*/ ) :
	HTreeWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, std::bind( &Impl::_start, impl.get() ) );
	connect( qtWindow, &UIMainWindow::signal_closeWindow, std::bind( &Impl::_closeWindow, impl.get() ) );
}

/////////////////////////////////////////
/// 新しいパスに移動
//void TreeViewFileBrowse::selectPath( const QString& path ) {
//	impl->selectPath( path );
//}

/////////////////////////////////////////
void TreeViewFileBrowse::addTop( QTreeWidgetItem* item ) {
	emit signal_addTop( item );
}

/////////////////////////////////////////
void TreeViewFileBrowse::mouseMoveEvent( QMouseEvent* e ) {
	qDebug() << e->button();
}

/////////////////////////////////////////
void TreeViewFileBrowse::mouseReleaseEvent( QMouseEvent* e ) {
	qDebug() << "release";
	HTreeWidget::mouseReleaseEvent(e);
}
