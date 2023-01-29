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


//////////////////////////////////////////////////////////////////////////////////
class UIViewR::Impl : public Ui_UIViewR {
public:
	using self_t = UIViewR;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	std::unique_ptr<ContextMenu> contextMenuFile;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

	}


	/////////////////////////////////////////
	void _start() {
		$::restoreState( treeWidget->header(), config.treeWidgetHeaderSize );

		utils::createContextMenu( &contextMenuFile, treeWidget );

		treeWidget->sortByColumn( eSize, Qt::SortOrder::DescendingOrder );

		$TreeWidget::itemDoubleClicked( treeWidget, std::bind( &Impl::_itemDoubleClicked, this ) );

		tabWidget->setCurrentIndex( 0 );

		// 左ビューの選択
		connect(
			qtWindow->uiViewL(),
			&UIViewL::itemSelectionChanged,
			std::bind( &Impl::uiViewL_itemSelectionChanged, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::signal_selectDrive,
			std::bind( &Impl::_selectDrive, this, std::placeholders::_1 ) );
	}


	/////////////////////////////////////////
	void _closeWindow() {
		$::saveState( config.treeWidgetHeaderSize, treeWidget->header() );
	}


	/////////////////////////////////////////
	void _selectDrive( QChar driveName ) {
		auto& dc = fileDB.driveCache( driveName );

		treeWidget_2->clear();

		for( auto& p : dc.symbolicLink.keys() ) {
			auto* iyem = new QTreeWidgetItem();

			iyem->setText( 0, p );
			iyem->setIcon( 0, icon::get( p ) );

			QFileInfo fi( p );
			if( fi.isJunction() ) {
				iyem->setText( 1, u8"ジャンクション" );
			}
			else if( fi.isShortcut() ) {
				iyem->setText( 1, u8"ショートカット" );
			}
			else {
				iyem->setText( 1, u8"シンボリックリンク" );
			}

			iyem->setText( 2, dc.symbolicLink[ p ] );
			iyem->setIcon( 2, icon::get( dc.symbolicLink[ p ] ) );

			treeWidget_2->addTopLevelItem( iyem );
		}
	}


	/////////////////////////////////////////
	/// ダブルクリック
	void _itemDoubleClicked() {
		auto* item = treeWidget->currentItem<ItemR>();
		if( !item ) return;

		if( item->isFolder() ) {
			emit self->itemDoubleClicked( item );
		}
		else {
			item->openFile();
		}
	}


	/////////////////////////////////////////
	void uiViewL_itemSelectionChanged( ItemL* item ) {
		treeWidget->clear();

		tabWidget->setCurrentIndex( 0 );

		if( !item )return;
		if( item->reference )return;

		for( auto& pathName : fs::getDirectories( item->fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
			QTreeWidgetItem* p = new ItemR( treeWidget, pathName );

			treeWidget->addTopLevelItem( p );
		}

		for( auto& pathName : fs::getFiles( item->fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
			auto* p = new ItemR( treeWidget, pathName );

			treeWidget->addTopLevelItem( p );
		}
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
