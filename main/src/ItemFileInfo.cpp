#include "ItemFileInfo.h"


//////////////////////////////////////////////////////////////////////////////////
ItemFileInfo::ItemFileInfo( QTreeWidget* parent, const QString& _fullPath ) :
	HTreeWidgetItem( parent ),
	fullPath( _fullPath ),
	fileInfo( _fullPath ) {

}


/////////////////////////////////////////
bool ItemFileInfo::openFile() {
	if( !fs::isExistFile( fullPath ) ) {
		HLogView::error( u8"ƒtƒ@ƒCƒ‹‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ: " + fullPath );
		return false;
	}
	$::showInExplorer( fullPath );
	return true;
}



/////////////////////////////////////////
bool ItemFileInfo::showInExplorer() {
	if( isFolder() || isRoot()) {
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
