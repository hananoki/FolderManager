#include "ItemFileInfo.h"


//////////////////////////////////////////////////////////////////////////////////
ItemFileInfo::ItemFileInfo( QTreeWidget* parent, const QString& _fullPath ) :
	HTreeWidgetItem( parent ),
	fullPath( _fullPath ),
	fileInfo( _fullPath ) 
{

}


/////////////////////////////////////////
bool ItemFileInfo::openFile() {
	if( fs::isExistFile( fullPath ) ) {
		$::showInExplorer( fullPath );
		return true;
	}
	if( fs::isExistDirectory( fullPath ) ) {
		$::showInExplorer( fullPath );
		return true;
	}
	UIStatusBar::error( u8"openFile: 処理出来ませんでした" + fullPath );
	return false;
}



/////////////////////////////////////////
bool ItemFileInfo::showInExplorer() {
	if( isFolder() || isRoot() ) {
		if( !fs::isExistDirectory( fullPath ) )return false;
		$::showInExplorer( fullPath );
	}
	else if( isFile() ) {
		auto path = path::getDirectoryName( fullPath );
		if( !fs::isExistDirectory( path ) )return false;
		$::showInExplorer( path );
	}


	return true;
}
