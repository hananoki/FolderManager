#include "UIViewL.h"
#include "ui_UIViewL.h"

#include "UIMainWindow.h"
#include "UIViewR.h"
#include "ItemL.h"
#include "ItemR.h"
#include "ContextMenu.h"
#include "ActionFile.h"
#include "FileDB.h"
#include "UIRowLinkItem.h"
#include "SelectiHistory.h"
#include "utils.h"

//////////////////////////////////////////////////////////////////////////////////
class UIViewL::Impl : public Ui_UIViewL {
public:
	using self_t = UIViewL;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	QFuture<void> future2;
	QFutureWatcher<void> watcher2;

	std::unique_ptr<ContextMenu> contextMenu;

	ItemL* itemRoot;

	QChar driveLetter;

	QHash<QString, ItemL*> drive2item;
	QHash<QString, ItemL*> cacheFileFolderItems;
	QHash<QString, QList<ItemL*>> driveItems;

	QString currentSelectPath;

	QMutex mutex;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

		treeWidget->sortByColumn( 0, Qt::SortOrder::AscendingOrder );

		/////////////////////////////////////////
		connect( &watcher2, &QFutureWatcher<void>::finished, self, std::bind( &Impl::watcher_finished, this ) );
	}


	/////////////////////////////////////////
	void watcher_finished() {
		self->repaint();
	}


	/////////////////////////////////////////
	void _start() {
		utils::createContextMenu( &contextMenu, treeWidget );

		$TreeWidget::itemSelectionChanged(
			treeWidget,
			std::bind( &Impl::_itemSelectionChanged, this ) );

		connect(
			treeWidget,
			&QTreeWidget::itemExpanded,
			std::bind( &Impl::itemExpanded, this, std::placeholders::_1 ) );

		connect(
			self,
			&self_t::signal_addTop,
			self,
			std::bind( &QTreeWidget::addTopLevelItem, treeWidget, std::placeholders::_1 ) );

		connect(
			self,
			&self_t::signal_addChild,
			self,
			std::bind( &Impl::addChild, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) );

		connect(
			self,
			&self_t::signal_setHidden,
			self,
			std::bind( &Impl::setHidden, this, std::placeholders::_1, std::placeholders::_2 ) );

		connect(
			qtWindow,
			&UIMainWindow::uiViewL_sortItem,
			self,
			std::bind( &Impl::uiViewL_sortItem, this ) );

		///新しいパスに移動
		connect(
			qtWindow,
			&UIMainWindow::signal_selectPath,
			self,
			std::bind( &Impl::selectPath, this, std::placeholders::_1 ) );

		/// 親アイテムに移動
		connect(
			qtWindow,
			&UIMainWindow::signal_moveParent,
			self,
			std::bind( &Impl::moveParent, this ) );

		/// フォルダ容量を計算
		connect(
			qtWindow,
			&UIMainWindow::signal_calcFolder,
			self,
			std::bind( &Impl::calcFolder, this ) );

		/// 指定したアイテムにフォーカス
		connect(
			self,
			&self_t::signal_focusItem,
			self,
			std::bind( &Impl::focusItem, this, std::placeholders::_1, std::placeholders::_2 ) );

		connect(
			qtWindow,
			&UIMainWindow::deleteItem,
			self,
			std::bind( &Impl::deleteItem, this, std::placeholders::_1 ) );

		/// キャッシュデータの読み込み完了
		connect(
			&fileDB,
			&FileDB::signal_completeFileLoad,
			self,
			std::bind( &Impl::signal_completeFileLoad, this ) );
	}


	/////////////////////////////////////////
	void setHidden( ItemL* i1, bool flg ) {
		i1->setHidden( flg );
	}


	/////////////////////////////////////////
	/// 親アイテムに移動
	void moveParent() {
		auto* item = treeWidget->currentItem<ItemL>();
		auto* parent = (ItemL*) item->parent();
		//parent->setExpanded(false);
		qtWindow->selectPath( parent->fullPath );
	}


	/////////////////////////////////////////
	/// フォルダ容量を計算
	void calcFolder() {
		auto* item = treeWidget->currentItem<ItemL>();
		if( !item )return;
		if( item->isSymbolicLink() ) {
			UIStatusBar::warning( u8"選択されたアイテムはシンボリックリンクです" );
			return;
		}

		auto button = QMessageBox::question( self, u8"確認", $$( u8"%1以下のフォルダ容量を計算します。\nよろしいですか？" ).arg( item->fullPath ) );
		if( button == QMessageBox::No )return;

		fileDB.analize( item->fullPath );
	}


	/////////////////////////////////////////
	void addChild( ItemL* p1, ItemL* p2, bool bCurrent /*= false*/ ) {
		p1->addChild( p2 );

		// ルートの時だけ展開
		// シグナルを発すると_itemSelectionChangedのアイテム生成を抑制したい
		// 理由としてルート直下のアイテムはchangeDriveで処理されるため無駄な処理となる
		if( p1->isRoot() ) {
			HSignalBlocker _( treeWidget );
			p1->setExpanded( true );
		}

		p2->addLinkWidget();

		if( bCurrent ) {
			treeWidget->setCurrentItem( p2 );
			treeWidget->scrollToItem( p2 );
		}

	}


	/////////////////////////////////////////
	/// 新しいパスに移動
	void selectPath( const QString& fullPath ) {

		if( currentSelectPath == fullPath )return;
		currentSelectPath = fullPath;

		if( future2.isRunning() ) {
			UIStatusBar::progressStart( u8"UIViewL: スレッドの同期待ち" );
			future2.waitForFinished();
			UIStatusBar::progressStop();
		}

		future2 = QtConcurrent::run( [&, fullPath]() {
			bool change = false;

			if( driveLetter != fullPath[ 0 ] ) {
				changeDrive( fullPath[ 0 ] );
				qtWindow->changeDrive( fullPath[ 0 ] );
				change = true;
			}

			QFileInfo fi( fullPath );

			itemRoot->selectPath( fullPath );

			selectiHistory.push( fullPath );

			self->focusItem( getCached( fullPath ), change );
		} );
		watcher2.setFuture( future2 );
	}


	/////////////////////////////////////////
	/// ドライブの切り替え
	void changeDrive( QChar _driveLetter ) {
		driveLetter = _driveLetter;

		for( auto* p : drive2item.values() ) {
			p->setHidden( true );
		}

		auto driveName = $$( "%1:" ).arg( _driveLetter );

		auto it = drive2item.find( driveName );
		if( it == drive2item.constEnd() ) {
			itemRoot = new ItemL( treeWidget, $$( "%1:/" ).arg( _driveLetter ) );
			drive2item.insert( driveName, itemRoot );
			self->addTop( itemRoot );

			itemRoot->makeChild( true );
		}
		else {
			self->setHidden( *it, false );
			itemRoot = ( *it );
		}
	}


	/////////////////////////////////////////
	/// 指定したアイテムにフォーカス
	void focusItem( ItemL* item, bool changedrive ) {
		if( changedrive ) {
			treeWidget->blockSignals( true );
		}
		treeWidget->setCurrentItem( item );
		treeWidget->scrollToItem( item );
		if( changedrive ) {
			treeWidget->blockSignals( false );
		}
	}


	/////////////////////////////////////////
	void deleteItem( const QString& fullPath ) {
		itemRoot->deletePath( fullPath );
	}


	/////////////////////////////////////////
	/// アイテムの選択切り替え
	void _itemSelectionChanged() {
		auto* item = treeWidget->currentItem<ItemL>();
		if( !item )return;

		//future2 = 
		//QtConcurrent::run( [&]() {
			//item->makeChild(false, true);
			//emit qtWindow->uiViewL_sortItem();
			//uiViewL_sortItem();
		//} );
		//watcher2.setFuture( future2 );

		emit self->signal_itemSelectionChanged( item );
		qtWindow->selectPath( item->fullPath );
	}


	/////////////////////////////////////////
	void itemExpanded( QTreeWidgetItem* item ) {
		auto* itemL = (ItemL*) item;

		if( future.isRunning() ) {
			future.waitForFinished();
		}

		future = QtConcurrent::run( [this, itemL]() {
			for( auto& p : itemL->childItems<ItemL>() ) {
				p->makeChild();
			}
			//emit qtWindow->uiViewL_sortItem();
			//uiViewL_sortItem();
		} );
		watcher.setFuture( future );
	}


	/////////////////////////////////////////
	void uiViewL_sortItem() {
		treeWidget->sortByColumn( 0, Qt::SortOrder::AscendingOrder );
	}


	/////////////////////////////////////////
	/// 右ビューのダブルクリック
	//void itemDoubleClicked( ItemR* itemR ) {
	//	qDebug() << itemR->fullPath;
	//	auto paths = itemR->fullPath.split( "/" );

	//	auto* item = treeWidget->findTopLevelItem<ItemL>( paths[ 0 ] + "/" );
	//	if( !item )return;
	//	paths.takeFirst();
	//	while( !paths.isEmpty() ) {
	//		item = item->find<ItemL>( paths[ 0 ] );
	//		if( !item )return;
	//		paths.takeFirst();
	//	}
	//	treeWidget->setCurrentItem( item );
	//}


	/////////////////////////////////////////
	/// キャッシュデータの読み込み完了
	void signal_completeFileLoad() {
		for( auto* p : drive2item.values() ) {
			p->addLinkWidget();
		}
		for( auto* p : cacheFileFolderItems.values() ) {
			p->addLinkWidget();
		}
	}


	/////////////////////////////////////////
	void createContextMenu() {
		treeWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		$TreeWidget::customContextMenuRequested( treeWidget, [&]( const QPoint& pos ) {
			QModelIndex index = treeWidget->indexAt( pos );
			if( !index.isValid() ) return;

			contextMenu = std::make_unique<ContextMenu>();

			auto* itemBase = reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() );

			auto* itemFileInfo = dynamic_cast<ItemFileInfo*>( itemBase );
			if( itemFileInfo ) {
				contextMenu->addAction<ActionDelete>( treeWidget );
			}

			//auto* item = dynamic_cast<ItemL*>( reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() ) );
			//if( item ) {

			//	contextMenu->addAction<Action_Analize>( treeWidget );
			//	//( *contextMenu )->addAction<ActionShowInExplorer>( treeWidget );
			//	//( *contextMenu )->addAction<ActionCopyFullPathName>( treeWidget );


			//}
			contextMenu->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		} );
	}


	/////////////////////////////////////////
	void setCache( const QString& path, ItemL* item ) {
		QMutexLocker _( &mutex );
		//auto it = driveItems.find( path[ 0 ] );
		//if( it == driveItems.constEnd() ) {
		//	driveItems.insert( path[ 0 ], QList<ItemL*>() );
		//	it = driveItems.find( path[ 0 ] );
		//}
		//( *it ) << item;
		cacheFileFolderItems.insert( path, item );
	}


	/////////////////////////////////////////
	ItemL* getCached( const QString& path ) {
		auto it = cacheFileFolderItems.find( path );
		return it != cacheFileFolderItems.constEnd() ? ( *it ) : nullptr;
	}

};


//////////////////////////////////////////////////////////////////////////////////
UIViewL::UIViewL( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, std::bind( &Impl::_start, impl.get() ) );
}


/////////////////////////////////////////
UIViewL::~UIViewL() {
}


////////////////////////
//void UIViewL::changeDrive( const QString& _driveLetter ) {
//	impl->changeDrive( _driveLetter );
//}

/////////////////////////////////////////
void UIViewL::setCache( const QString& path, ItemL* item ) {
	impl->setCache( path, item );
}


/////////////////////////////////////////
ItemL* UIViewL::getCached( const QString& path ) {
	return impl->getCached( path );
}


/////////////////////////////////////////
//bool UIViewL::isBusy() {
//	return 0 < impl->addCound;
//}


/////////////////////////////////////////
//void UIViewL::selectPath( const QString& path ) {
//	return impl->selectPath( path );
//}

/////////////////////////////////////////
void UIViewL::addTop( ItemL* i1 ) {
	emit signal_addTop( i1 );
}

/////////////////////////////////////////
void UIViewL::addChild( ItemL* i1, ItemL* i2, bool bCurrent /*= false*/ ) {
	emit signal_addChild( i1, i2, bCurrent );
}

/////////////////////////////////////////
void UIViewL::setHidden( ItemL* i1, bool flg ) {
	emit signal_setHidden( i1, flg );
}

/////////////////////////////////////////
void UIViewL::focusItem( ItemL* item, bool changedrive ) {
	emit signal_focusItem( item, changedrive );
}
