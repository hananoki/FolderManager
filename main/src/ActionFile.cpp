#include "ActionFile.h"
#include "ItemR.h"
#include "ItemL.h"


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
			HLogView::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionShowInExplorer::ActionShowInExplorer( HTreeWidget* tw ) :
	QAction( tr( u8"ファイルの場所を開く" ) ),
	treeWidget( tw ) {

	QObject::connect( this, &QAction::triggered, [&]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		if( item ) {
			item->showInExplorer();
		}
		else {
			HLogView::error( u8"選択アイテムエラー" );
		}
	} );
}
ActionShowInExplorer::~ActionShowInExplorer() {
	qDebug() << "";
}


//////////////////////////////////////////////////////////////////////////////////
ActionCopyFileName::ActionCopyFileName( HTreeWidget* tw ) :
	QAction( tr( u8"フルパスをクリップボードにコピー" ) ),
	treeWidget( tw ) {

	connect( this, &QAction::triggered, [&]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		if( item ) {
			QClipboard* clipboard = QApplication::clipboard();
			clipboard->setText( item->fullPath );
		}
		else {
			HLogView::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionDelete::ActionDelete( HTreeWidget* tw ) :
	QAction( tr( u8"削除" ) ),
	treeWidget( tw ) {

	connect( this, &QAction::triggered, [&]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		//if( item ) {
		//	QClipboard* clipboard = QApplication::clipboard();
		//	clipboard->setText( item->fullPath );
		//}
		//else {
		//	HLogView::error( u8"選択アイテムエラー" );
		//}
		UIStatusBar::info( u8"Unimplemented" );
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionRename::ActionRename( HTreeWidget* tw ) :
	QAction( tr( u8"名前の変更" ) ),
	treeWidget( tw ) {

	connect( this, &QAction::triggered, [&]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		//if( item ) {
		//	QClipboard* clipboard = QApplication::clipboard();
		//	clipboard->setText( item->fullPath );
		//}
		//else {
		//	HLogView::error( u8"選択アイテムエラー" );
		//}
		UIStatusBar::info( u8"Unimplemented" );
	} );
}


//////////////////////////////////////////////////////////////////////////////////
ActionShowProperty::ActionShowProperty( HTreeWidget* tw ) :
	QAction( tr( u8"プロパティ" ) ),
	treeWidget( tw ) {

	connect( this, &QAction::triggered, [&]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		if( item ) {
			$::showProperty( item->fullPath );
		}
		else {
			HLogView::error( u8"選択アイテムエラー" );
		}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
Action_Analize::Action_Analize( HTreeWidget* treeWidget ) :
	QAction( tr( u8"このフォルダ以下を更新する" ) ) {

	//setIcon( ICON_EXPLORER );

	connect( this, &QAction::triggered, [treeWidget]() {
		//	auto* item = treeWidget->currentItem<ItemR>();
		//	if( item ) {
		//		item->openFile();
		//	}
		//	else {
		//		HLogView::error( u8"選択アイテムエラー" );
		//	}
	} );
}


//////////////////////////////////////////////////////////////////////////////////
Action_RemoveFolder::Action_RemoveFolder( HTreeWidget* treeWidget ) :
	QAction( tr( u8"削除" ) ) {

	//setIcon( ICON_EXPLORER );

	connect( this, &QAction::triggered, [treeWidget]() {
		//	auto* item = treeWidget->currentItem<ItemR>();
		//	if( item ) {
		//		item->openFile();
		//	}
		//	else {
		//		HLogView::error( u8"選択アイテムエラー" );
		//	}
	} );
}

Action_RemoveFolder* Action_RemoveFolder::create( HTreeWidget* p ) {
	auto item = p->currentItem<ItemFileInfo>();
	if( !item )return nullptr;

	if( item->isRoot() ) {
		return nullptr;
	}
	return new Action_RemoveFolder( p );
}