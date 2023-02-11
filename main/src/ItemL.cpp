#include "ItemL.h"

#include "FileDB.h"

#include "UIMainWindow.h"
#include "UIViewL.h"
#include "UIRowLinkItem.h"

using namespace cpplinq;

//////////////////////////////////////////////////////////////////////////////////
ItemL::ItemL( HTreeWidget* parent, const QString& _fullPath ) :
	ItemFileInfo( parent, _fullPath ) {

	setCache( _fullPath, this );

	setText( 0, fileInfo.isRoot() ?
		$$( "%1 (%2:)" ).arg( fs::volumeName( fullPath ) ).arg( fullPath[ 0 ] ) :
		path::getFileName( fullPath ) );

	if( fileInfo.isLink() ) {
		setIcon( 0, ICON_FOLDER );
	}
	else {
		setIcon( 0, icon::get( fullPath ) );
	}
}


/////////////////////////////////////////
/**
 * @brief  �q�A�C�e�����쐬����
 * @param  bInit �h���C�u���[�g�p�A���[�g�̏ꍇ�A���������q�A�C�e���̎q�����
 * @param  �������[�h�A����������addChild����ꍇ�A�܂Ƃ߂�addChildren����ق����\�[�g�񐔂����邽�ߑ����Ȃ�
 */
void ItemL::makeChild( bool bInit /*= false*/, bool syncMode /*= false*/ ) {
	if( isReference() )return;

	//int count = 0;
	QList<QTreeWidgetItem*> items;
	fs::enumerateDirectories( fullPath, "*", SearchOption::TopDirectoryOnly, QDir::Hidden, [&]( const QString& pathName ) {

		if( getCached( pathName ) )return true;

		auto* p = new ItemL( tw, pathName );

		if( syncMode ) {
			items << p;
		}
		else {
			qtWindow->uiViewL()->addChild( this, p );
		}

		if( bInit ) {
			p->makeChild();
		}
		//count++;
		return true;
	} );

	if( syncMode ) {
		addChildren( items );
	}
}


/////////////////////////////////////////
void ItemL::selectPath( const QString& path ) {
	if( !isRoot() ) {
		UIStatusBar::error( u8"���[�g�ł͂Ȃ�" );
		return;
	}

	auto paths = from( path.split( "/" ) )
		>> where( []( const auto& s ) { return !s.isEmpty(); } )
		>> to_qlist();

	QString tPath = paths.takeFirst();
	auto* itemL = this;

	while( !paths.isEmpty() ) {
		auto dname = paths.takeFirst();
		tPath = $$( "%1/%2" ).arg( tPath ).arg( dname );
		auto* child = getCached( tPath );
		if( !fs::isExistDirectory( tPath ) ) {
			break;
		}
		if( !child ) {
			child = new ItemL( tw, tPath );
			qtWindow->uiViewL()->addChild( itemL, child );
			//itemL->addChild( child );
		}
		itemL = child;
	}

	if( !QFileInfo( path ).isRoot() ) {
		//emit qtWindow->uiViewL()->focusItem( itemL );
	}
}


/////////////////////////////////////////
void ItemL::deletePath( const QString& path ) {
	if( !isRoot() ) {
		UIStatusBar::error( u8"���[�g�ł͂Ȃ�" );
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
		//	UIStatusBar::error( u8"�w�肳�ꂽ�p�X��������Ȃ�: " + tPath );
		//	return;
		//}
		if( !child ) {
			UIStatusBar::error( u8"�w�肳�ꂽ�A�C�e����������Ȃ�: " + tPath );
			return;
		}
		itemL = child;
	}
	itemL->setHidden( true );
}


/////////////////////////////////////////
void ItemL::addLinkWidget() {
	auto lst = fileDB.symbolicLinkSource( fullPath );
	if( 0 < lst.length() ) {
		auto* bb = new UIRowLinkItem( tw, lst );
		tw->setItemWidget( this, 0, bb );
	}
}


/////////////////////////////////////////
void ItemL::setCache( const QString& path, ItemL* item ) {
	qtWindow->uiViewL()->setCache( path, item );
}

/////////////////////////////////////////
ItemL* ItemL::getCached( const QString& path ) {
	return qtWindow->uiViewL()->getCached( path );
}
