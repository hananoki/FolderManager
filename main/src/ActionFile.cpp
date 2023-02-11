#include "ActionFile.h"

#include "UIMainWindow.h"
#include "ItemR.h"
#include "ItemL.h"
#include "ItemSymLink.h"

#ifdef Q_OS_WIN
#include <QtWin>
#include <shlwapi.h>
#endif

//////////////////////////////////////////////////////////////////////////////////
ActionOpenFile::ActionOpenFile( HTreeWidget* treeWidget ) :
	QAction( tr( u8"開く" ) ) {

	setIcon( ICON_EXPLORER );

	QObject::connect( this, &QAction::triggered, [treeWidget]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		if( item ) {
			item->openFile();
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionShowInExplorer::ActionShowInExplorer( HTreeWidget* tw ) :
	QAction() {

	auto* item = tw->currentItem<ItemFileInfo>();
	if( item->isFile() ) {
		setText( tr( u8"ファイルの場所を開く" ) );
	}
	else {
		setText( tr( u8"フォルダの場所を開く" ) );
	}

	QObject::connect( this, &QAction::triggered, [&, tw]() {
		auto* item = tw->currentItem<ItemFileInfo>();
		if( item ) {
			item->showInExplorer();
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionCopyPathName::ActionCopyPathName( HTreeWidget* tw ) :
	QAction() {

	//setIcon( QIcon( ":/res/icon/clipboard.png" ) );

	auto* item = tw->currentItem<ItemFileInfo>();
	if( item->isFile() ) {
		setText( tr( u8"ファイル名をクリップボードにコピー" ) );
	}
	else {
		setText( tr( u8"フォルダ名をクリップボードにコピー" ) );
	}

	connect( this, &QAction::triggered, [&, tw]() {
		auto* item = tw->currentItem<ItemFileInfo>();
		if( item ) {
			QClipboard* clipboard = QApplication::clipboard();
			clipboard->setText( path::getFileName( item->fullPath ) );
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionCopyFullPathName::ActionCopyFullPathName( HTreeWidget* tw ) :
	QAction( tr( u8"フルパスをクリップボードにコピー" ) ) {

	//setIcon( QIcon( ":/res/icon/clipboard.png" ) );

	connect( this, &QAction::triggered, [&, tw]() {
		auto* item = tw->currentItem<ItemFileInfo>();
		if( item ) {
			QClipboard* clipboard = QApplication::clipboard();
			clipboard->setText( item->fullPath );
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionDelete::ActionDelete( HTreeWidget* tw ) :
	QAction( tr( u8"削除" ) ) {

	setIcon( QIcon( ":/res/icon/garbageCan.png" ) );

	connect( this, &QAction::triggered, [&, tw]() {
		auto* item = tw->currentItem<ItemFileInfo>();
		int result = fs::moveToTrash( item->fullPath );
		
		if( result == 0 ) {
			emit qtWindow->deleteItem( item->fullPath );
			UIStatusBar::info( u8"削除しました: " + item->fullPath );
		}
		else {
			UIStatusBar::info( $$( u8"moveToTrash: return code %1" ).arg( result ) );
		}
	} );
}


/////////////////////////////////////////
ActionDelete* ActionDelete::create( HTreeWidget* p ) {
	auto item = p->currentItem<ItemFileInfo>();
	if( !item )return nullptr;

	if( item->isRoot() ) {
		return nullptr;
	}
	return new ActionDelete( p );
}


//////////////////////////////////////////////////////////////////////////////////
ActionRename::ActionRename( HTreeWidget* tw ) :
	QAction( tr( u8"名前の変更" ) ) {

	connect( this, &QAction::triggered, [tw]() {
		auto* item = tw->currentItem<ItemFileInfo>();
		if( item ) {
			tw->editItem( item, 0 );
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionShowProperty::ActionShowProperty( HTreeWidget* tw ) :
	QAction( tr( u8"プロパティ" ) ) {

	connect( this, &QAction::triggered, [tw]() {
		auto* item = tw->currentItem<ItemFileInfo>();
		if( item ) {
			$::showProperty( item->fullPath );
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
//Action_Analize::Action_Analize( HTreeWidget* treeWidget ) :
//	QAction( tr( u8"このフォルダ以下を更新する" ) ) {
//
//	//setIcon( ICON_EXPLORER );
//
//	connect( this, &QAction::triggered, [treeWidget]() {
//		//	auto* item = treeWidget->currentItem<ItemR>();
//		//	if( item ) {
//		//		item->openFile();
//		//	}
//		//	else {
//		//		HLogView::error( u8"選択アイテムエラー" );
//		//	}
//	} );
//}


//////////////////////////////////////////////////////////////////////////////////
Action_ShowTargetPath::Action_ShowTargetPath( HTreeWidget* treeWidget ) :
	QAction( tr( u8"ターゲットパスのアイテムを選択" ) ) {

	//setIcon( ICON_EXPLORER );

	connect( this, &QAction::triggered, [treeWidget]() {
		auto* item = treeWidget->currentItem<ItemSymLink>();
		if( item ) {
			qtWindow->selectPath( item->targegtPath );
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
Action_LnkFile::Action_LnkFile( HTreeWidget* treeWidget, const QString& filePath ) :
	QAction( path::getBaseName( filePath ) ) {

	setIcon( icon::get( filePath ) );

	connect( this, &QAction::triggered, [treeWidget, filePath]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		if( item ) {
			auto bak = QDir::currentPath();
			QDir::setCurrent( path::getDirectoryName( item->fullPath ) );
			$::shellExecute( filePath, item->fullPath );
			QDir::setCurrent( bak );
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}
