#include "ItemFileInfo.h"


//////////////////////////////////////////////////////////////////////////////////
ItemFileInfo::ItemFileInfo( HTreeWidget* parent, const QString& _fullPath ) :
	HTreeWidgetItem(),
	fullPath( _fullPath ),
	fileInfo( _fullPath ),
	tw( parent ) {

}


/////////////////////////////////////////
bool ItemFileInfo::openFile() {
	if( fs::isExistFile( fullPath ) || fs::isExistDirectory( fullPath ) ) {
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
