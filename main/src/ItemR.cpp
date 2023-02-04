#include "ItemR.h"
#include "FileDB.h"
#include "utils.h"



//////////////////////////////////////////////////////////////////////////////////
ItemR::ItemR( QTreeWidget* parent, const QString& _fullPath, bool bFullName /*= false*/ ) :
	ItemFileInfo( parent, _fullPath ),
	size( -1 ) {

	if( bFullName ) {
		setText( eName, fullPath );
	}
	else {
		setText( eName, path::getFileName( fullPath ) );
	}

	if( fs::isExistDirectory( _fullPath ) ) {
		bool b1 = fileInfo.isSymLink();
		bool b2 = fileInfo.isSymbolicLink();
		bool b3 = fileInfo.isShortcut();
		bool b4 = fileInfo.isJunction();

		if( b1 || b2 || b3 || b4 ) {
			setIcon( 0, ICON_FOLDER );
			//auto te = text(0);
			if( b4 ) {
				setText( eType, u8"ジャンクション" );
				setText( eName, $$( "%1 (%2)" ).arg( path::getFileName( fullPath ) ).arg( fileInfo.linkTarget() ) );
			}
			else if( b3 ) {
				setText( eType, u8"ショートカット" );
			}
			else {
				setText( eType, u8"シンボリックリンク" );
				if( ( b1 & b2 ) == false ) {
					qDebug() << "";
				}

				fileDB.setSymbolicLink( fullPath, fileInfo.symLinkTarget() );
				setText( eName, $$( "%1 (%2)" ).arg( path::getFileName( fullPath ) ).arg( fileInfo.linkTarget() ) );
			}
		}
		else {
			setIcon( eName, icon::get( fullPath ) );
			setText( eType, u8"フォルダ" );
			setText( eDate, $::toString( fileInfo.lastModified() ) );

			size = fileDB.get( fullPath );
			setText( eSize, $::toStringFileSize( fileDB.get( fullPath ) ) );
			//setText( 1, $::toString( fileDB.get( fullPath ) ) );
			setTextAlignment( eSize, Qt::AlignRight );
		}
	}
	else {

		setIcon( eName, icon::get( fullPath ) );

		size = fileInfo.size();
		setText( eDate, $::toString( fileInfo.lastModified() ) );
		setText( eType, $::fileKind( fullPath ) );
		setText( eSize, $::toStringFileSize( fileInfo.size() ) );

		//setText( 1, $::toString( QFileInfo( fullPath ).size() ) );
		setTextAlignment( eSize, Qt::AlignRight );
	}
}


/////////////////////////////////////////
bool ItemR::openFile() {
	if( !fs::isExistFile( fullPath ) ) {
		HLogView::error( u8"ファイルが見つかりません: " + fullPath );
		return false;
	}
	$::showInExplorer( fullPath );
	return true;
}


/////////////////////////////////////////
QString ItemR::fileName() {
	return path::getFileName( fullPath );
}


/////////////////////////////////////////
QString ItemR::folderPath() {
	return path::getDirectoryName( fullPath );
}




//////////////////////////////////////////////////////////////////////////////////
ItemSymLink::ItemSymLink( QTreeWidget* parent, const QString& _fullPath, const QString& _targegtPath ) :
	ItemFileInfo( parent, _fullPath ),
	targegtPath( _targegtPath ) {

	setText( 0, _fullPath );
	setIcon( 0, icon::get( _fullPath ) );

	QFileInfo fi( _fullPath );
	if( fi.isJunction() ) {
		setText( 1, u8"ジャンクション" );
	}
	else if( fi.isShortcut() ) {
		setText( 1, u8"ショートカット" );
	}
	else {
		setText( 1, u8"シンボリックリンク" );
	}

	setText( 2, _targegtPath );

	if( !fs::isExistDirectory( _targegtPath ) ) {
		setBackgroundColor( 2, QColor( "#FBB" ) );
	}
	//else {
	setIcon( 2, icon::get( _targegtPath ) );
	//}
}
