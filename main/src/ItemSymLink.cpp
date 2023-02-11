#include "ItemSymLink.h"

//////////////////////////////////////////////////////////////////////////////////
ItemSymLink::ItemSymLink( HTreeWidget* parent, const QString& _fullPath, const QString& _targegtPath ) :
	ItemFileInfo( parent, _fullPath ),
	targegtPath( _targegtPath ) {

	setText( 0, _fullPath );
	setIcon( 0, icon::get( _fullPath ) );

	QFileInfo fi( _fullPath );
	if( fi.isJunction() ) {
		setText( 1, u8"�W�����N�V����" );
	}
	else if( fi.isShortcut() ) {
		setText( 1, u8"�V���[�g�J�b�g" );
	}
	else {
		setText( 1, u8"�V���{���b�N�����N" );
	}

	setText( 2, _targegtPath );

	if( !fs::isExistDirectory( _targegtPath ) ) {
		setBackgroundColor( 2, QColor( "#FBB" ) );
	}
	//else {
	setIcon( 2, icon::get( _targegtPath ) );
	//}
}

