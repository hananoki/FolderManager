#include "ItemL.h"

#include "FileDB.h"

#include "UIMainWindow.h"
#include "UIViewL.h"

using namespace cpplinq;

//////////////////////////////////////////////////////////////////////////////////
ItemL::ItemL( QTreeWidget* parent, const QString& _fullPath ) :
	ItemFileInfo( parent, _fullPath ),
	mkChild( false ) {

	setText( 0, fileInfo.isRoot() ? fullPath : path::getFileName( fullPath ) );
	
	if( fileInfo.isLink() ) {
		setIcon( 0, ICON_FOLDER );
		reference = true;
	}
	else {
		setIcon( 0, icon::get( fullPath ) );
		reference = false;
	}

	auto lst = fileDB.symbolicLinkSource( fullPath );
	if( 0 < lst.length() ) {
		setIcon( 1, QIcon( ":/res/icon/link.png" ) );
	}
}


/////////////////////////////////////////
void ItemL::makeChild( bool bInit /*= false*/ ) {
	if( reference )return;
	if( mkChild )return;

	int count = 0;
	QList<QTreeWidgetItem*> items;
	fs::enumerateDirectories( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden, [&]( const QString& pathName ) {
		auto* fitem = find<ItemL>( path::getFileName( pathName ) );
		if( fitem )return true;

		auto* p = new ItemL( nullptr, pathName );

		//emit qtWindow->uiViewL_addChild( this, p );
		//addChild( p );
		items << p;

		if( bInit ) {
			p->makeChild();
		}
		count++;
		return true;
	} );
	addChildren( items );

}


/////////////////////////////////////////
void ItemL::selectPath( const QString& path ) {
	if( !isRoot() ) {
		UIStatusBar::error( u8"ルートではない" );
		return;
	}

	auto paths = from( path.split( "/" ) )
		>> where( []( const auto& s ) { return !s.isEmpty(); } )
		>> to_qlist();
		
	QString tPath = paths.takeFirst();
	auto* itemL = this;

	while( !paths.isEmpty() ) {
		auto dname = paths.takeFirst();
		auto* child = itemL->find<ItemL>( dname );
		tPath = $$( "%1/%2" ).arg( tPath ).arg( dname );
		if( !fs::isExistDirectory( tPath ) ) {
			break;
		}
		if( !child ) {
			child = new ItemL( nullptr, tPath );
			//emit qtWindow->uiViewL_addChild( itemL, child, true );
			itemL->addChild( child );
		}
		itemL = child;
	}
	emit qtWindow->uiViewL_focusItem( itemL );
}


/////////////////////////////////////////
void ItemL::deletePath( const QString& path ) {
	if( !isRoot() ) {
		UIStatusBar::error( u8"ルートではない" );
		return;
	}

	auto paths = path.split( "/" );

	QString tPath = paths.takeFirst();
	auto* itemL = this;

	while( !paths.isEmpty() ) {
		auto dname = paths.takeFirst();
		auto* child = itemL->find<ItemL>( dname );
		tPath = $$( "%1/%2" ).arg( tPath ).arg( dname );
		//if( !fs::isExistDirectory( tPath ) ) {
		//	UIStatusBar::error( u8"指定されたパスが見つからない: " + tPath );
		//	return;
		//}
		if( !child ) {
			UIStatusBar::error( u8"指定されたアイテムが見つからない: " + tPath );
			return;
		}
		itemL = child;
	}
	itemL->setHidden(true);
}
