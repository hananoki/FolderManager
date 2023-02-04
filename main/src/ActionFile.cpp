#include "ActionFile.h"
#include "ItemR.h"
#include "ItemL.h"
#include "UIMainWindow.h"
#include "UIViewL.h"
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

	//setIcon( icon::SP_TrashIcon() );
	setIcon( QIcon( ":/res/icon/garbageCan.png" ) );

	connect( this, &QAction::triggered, [&]() {
		auto* item = treeWidget->currentItem<ItemFileInfo>();
		if( item->isFolder() ) {
			int result = fs::moveToTrash( item->fullPath );
			if( result == 0 ) {
				emit qtWindow->deleteFolder( item->fullPath );
				UIStatusBar::info( u8"削除しました: " + item->fullPath );
			}
			else {
				UIStatusBar::info( $$( u8"moveToTrash: return code %1" ).arg( result ) );
			}
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
			UIStatusBar::error( u8"選択アイテムエラー" );
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
Action_ShowTargetPath::Action_ShowTargetPath( HTreeWidget* treeWidget ) :
	QAction( tr( u8"ターゲットパスのアイテムを選択" ) ) {

	//setIcon( ICON_EXPLORER );

	connect( this, &QAction::triggered, [treeWidget]() {
		auto* item = treeWidget->currentItem<ItemSymLink>();
		if( item ) {
			//$::showInExplorer( item->targegtPath );
			emit qtWindow->uiViewL_selectPath( item->targegtPath );
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
		auto* item = treeWidget->currentItem<ItemR>();
		if( item ) {
			//	//$::showInExplorer( item->targegtPath );
			//	emit qtWindow->uiViewL_selectPath( item->targegtPath );
			ShellExecute( NULL, L"open", $::toWCharPtr( filePath ), $::toWCharPtr( path::separatorToOS( item->fullPath ) ), NULL, SW_SHOWNORMAL );
			/*QProcess p;
			p.setWorkingDirectory( item->folderPath() );
			p.startDetached( filePath, { path::separatorToOS( item->fullPath ) } );*/
		}
		else {
			UIStatusBar::error( u8"選択アイテムエラー" );
		}
	} );
}
