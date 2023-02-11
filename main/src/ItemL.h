#pragma once

#include "ItemFileInfo.h"

//////////////////////////////////////////////////////////////////////////////////
class ItemL : public ItemFileInfo {
public:
	/////////////////////////////////////////
	ItemL( HTreeWidget* parent, const QString& _fullPath );

	/////////////////////////////////////////
	void makeChild( bool bInit = false, bool syncMode = false );

	/////////////////////////////////////////
	void selectPath( const QString& path );

	/////////////////////////////////////////
	void deletePath( const QString& path );

	/////////////////////////////////////////
	void addLinkWidget();

	bool isReference();

private:
	void setCache( const QString& path, ItemL* item );
	ItemL* getCached( const QString& path );
};


inline
bool ItemL::isReference() {
	return fileInfo.isLink();
}
