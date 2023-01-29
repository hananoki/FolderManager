#include "ItemL.h"

#include "FileDB.h"

#include "UIMainWindow.h"


//////////////////////////////////////////////////////////////////////////////////
ItemL::ItemL( QTreeWidget* parent, const QString& _fullPath ) :
	ItemFileInfo( parent, _fullPath ),
	mkChild( false ) {

	bool isRoot = QDir( fullPath ).isRoot();

	setText( 0, isRoot ? fullPath : path::getFileName( fullPath ) );

	QFileInfo finfo( _fullPath );
	bool b1 = finfo.isSymLink();
	bool b2 = finfo.isSymbolicLink();
	bool b3 = finfo.isShortcut();
	bool b4 = finfo.isJunction();

	if( finfo.symLinkTarget().isEmpty() && !b4 ) {
		setIcon( 0, icon::get( fullPath ) );
		reference = false;
	}
	else {
		setIcon( 0, ICON_FOLDER );
		reference = true;
	}

	auto lst = fileDB.symbolicLinkSource( fullPath );
	if(0 < lst.length() ) {
		setIcon( 1, QIcon(":/res/icon/link.png") );
	}
}


/////////////////////////////////////////
void ItemL::makeChild() {
	if( reference )return;
	if( mkChild )return;

	for( auto& pathName : fs::getDirectories( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden ) ) {
		auto* p = new ItemL( nullptr, pathName );

		emit qtWindow->signal_addItem( this, p );
	}
	mkChild = true;
}

