#pragma once

#include "ItemFileInfo.h"

enum {
	eName = 0,
	eDate = 1,
	eType = 2,
	eSize = 3,
};

class ItemR : public ItemFileInfo {
public:
	ItemR( QTreeWidget* parent, const QString& _fullPath );
	//TreeWidgetItem( QTreeWidget* parent ) :QTreeWidgetItem( parent ) {}


	qint64 size;

	/////////////////////////////////////////
	//bool showInExplorer();
	bool openFile();

	/////////////////////////////////////////
	QString fileName();

	/////////////////////////////////////////
	QString folderPath();

	

	bool operator<( const QTreeWidgetItem& other )const {

		int column = treeWidget()->sortColumn();
		if( column == 0 ) {
			return text( column ).toLower() < other.text( column ).toLower();
		}
		
		//QCollator collator;
		//collator.setNumericMode( true );
		//return collator.compare( text( size ), other.text( size ) ) < 0;
		return size < ((ItemR&)other).size;
	}
};

