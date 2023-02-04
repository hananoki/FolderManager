#pragma once

#include "ItemFileInfo.h"

enum {
	eName = 0,
	eDate = 1,
	eType = 2,
	eSize = 3,
};


//////////////////////////////////////////////////////////////////////////////////
class ItemR : public ItemFileInfo {
public:
	ItemR( QTreeWidget* parent, const QString& _fullPath, bool bFullName = false );

	qint64 size;

	/////////////////////////////////////////
	//bool showInExplorer();
	bool openFile();

	/////////////////////////////////////////
	QString fileName();

	/////////////////////////////////////////
	QString folderPath();


	/////////////////////////////////////////
	bool operator<( const QTreeWidgetItem& other )const {

		int column = treeWidget()->sortColumn();
		if( column == eName || column == eType ) {
			return text( column ).toLower() < other.text( column ).toLower();
		}

		//QCollator collator;
		//collator.setNumericMode( true );
		//return collator.compare( text( size ), other.text( size ) ) < 0;
		return size < ( (ItemR&) other ).size;
	}
};


//////////////////////////////////////////////////////////////////////////////////
class ItemSymLink : public ItemFileInfo {
public:
	ItemSymLink( QTreeWidget* parent, const QString& _fullPath, const QString& _targegtPath );

	QString targegtPath;
};
