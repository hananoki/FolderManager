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


//////////////////////////////////////////////////////////////////////////////////
class UIViewL::Impl : public Ui_UIViewL {
public:
	using self_t = UIViewL;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	std::unique_ptr<ContextMenu> contextMenu;

	ItemL* itemRoot;

	QChar driveLetter;


	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

		treeWidget->sortByColumn( 0, Qt::SortOrder::AscendingOrder );

		/////////////////////////////////////////
		//connect( &watcher, &QFutureWatcher<void>::finished, self, std::bind( &Impl::watcher_finished, this ) );
	}


	/////////////////////////////////////////
	void _start() {
		createContextMenu();

		$TreeWidget::itemSelectionChanged(
			treeWidget,
			std::bind( &Impl::_itemSelectionChanged, this ) );

		connect(
			treeWidget,
			&QTreeWidget::itemExpanded,
			std::bind( &Impl::itemExpanded, this, std::placeholders::_1 ) );



		/// 右ビューのダブルクリック
		//connect(
		//	qtWindow->uiViewR(),
		//	&UIViewR::itemDoubleClicked,
		//	std::bind( &Impl::itemDoubleClicked, this, std::placeholders::_1 ) );

		//connect(
		//	qtWindow,
		//	&UIMainWindow::changeDrive,
		//	std::bind( &Impl::changeDrive, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::uiViewL_addChild,
			self,
			std::bind( &Impl::uiViewL_addChild, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) );

		connect(
			qtWindow,
			&UIMainWindow::uiViewL_sortItem,
			self,
			std::bind( &Impl::uiViewL_sortItem, this ) );

		///新しいパスに移動
		connect(
			qtWindow,
			&UIMainWindow::uiViewL_selectPath,
			self,
			std::bind( &Impl::selectPath, this, std::placeholders::_1 ) );

		/// 親アイテムに移動
		connect(
			qtWindow,
			&UIMainWindow::action_moveParent,
			self,
			std::bind( &Impl::action_moveParent, this ) );

		/// フォルダ容量を計算
		connect(
			qtWindow,
			&UIMainWindow::action_calcFolder,
			self,
			std::bind( &Impl::action_calcFolder, this ) );

		/// 指定したアイテムにフォーカス
		connect(
			qtWindow,
			&UIMainWindow::uiViewL_focusItem,
			self,
			std::bind( &Impl::uiViewL_focusItem, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::deleteFolder,
			self,
			std::bind( &Impl::deleteFolder, this, std::placeholders::_1 ) );
	}


	/////////////////////////////////////////
	/// 親アイテムに移動
	void action_moveParent() {
		auto* item = treeWidget->currentItem<ItemL>();
		auto* parent = (ItemL*) item->parent();
		//parent->setExpanded(false);
		emit qtWindow->uiViewL_selectPath( parent->fullPath );
	}


	/////////////////////////////////////////
	/// フォルダ容量を計算
	void action_calcFolder() {
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
	void uiViewL_addChild( ItemL* p1, ItemL* p2, bool bCurrent /*= false*/ ) {
		p1->addChild( p2 );

		auto lst = fileDB.symbolicLinkSource( p2->fullPath );
		if( 0 < lst.length() ) {
			auto* bb = new UIRowLinkItem( treeWidget, lst );
			treeWidget->setItemWidget( p2, 0, bb );
		}
		if( bCurrent ) {
			treeWidget->setCurrentItem( p2 );
			treeWidget->scrollToItem( p2 );
		}

	}


	/////////////////////////////////////////
	///新しいパスに移動
	void selectPath( QString path ) {
		if( driveLetter != path[ 0 ] ) {
			changeDrive( path[ 0 ] );
			emit qtWindow->changeDrive( path[ 0 ] );
		}
		itemRoot->selectPath( path );

		selectiHistory.push( path );
	}


	/////////////////////////////////////////
	/// 指定したアイテムにフォーカス
	void uiViewL_focusItem( ItemL* item ) {
		treeWidget->setCurrentItem( item );
		treeWidget->scrollToItem( item );
	}


	/////////////////////////////////////////
	void deleteFolder( const QString& fullPath ) {
		itemRoot->deletePath( fullPath );
	}


	/////////////////////////////////////////
	/// アイテムの選択切り替え
	void _itemSelectionChanged() {
		auto* item = treeWidget->currentItem<ItemL>();
		if( !item )return;

		//QtConcurrent::run( [item]() {
			item->makeChild();
			//emit qtWindow->uiViewL_sortItem();
			uiViewL_sortItem();
		//} );

		emit self->itemSelectionChanged( item );
		emit qtWindow->uiViewL_selectPath( item->fullPath );
	}


	/////////////////////////////////////////
	void itemExpanded( QTreeWidgetItem* item ) {
		auto* itemL = (ItemL*) item;
		//future = QtConcurrent::run( [itemL]() {
			for( auto& p : itemL->childItems<ItemL>() ) {
				p->makeChild();
			}
			//emit qtWindow->uiViewL_sortItem();
			uiViewL_sortItem();
		//} );
		//watcher.setFuture( future );
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
			//	//( *contextMenu )->addAction<ActionCopyFileName>( treeWidget );


			//}
			contextMenu->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		} );
	}


	/////////////////////////////////////////
	/// ドライブの切り替え
	void changeDrive( QChar _driveLetter ) {
		driveLetter = _driveLetter;
		treeWidget->clear();

		itemRoot = new ItemL( treeWidget, $$( "%1:/" ).arg( _driveLetter ) );

		treeWidget->addTopLevelItem( itemRoot );

		//QtConcurrent::run( [&]() {
			itemRoot->makeChild( true );
		//} );

		itemRoot->setExpanded( true );
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
