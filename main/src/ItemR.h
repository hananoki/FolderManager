#pragma once

#include "ItemFileInfo.h"
#include "Enums.h"


//////////////////////////////////////////////////////////////////////////////////
class ItemR : public ItemFileInfo {
public:
	ItemR( HTreeWidget* parent, const QString& _fullPath, bool bFullName = false );

	qint64 size;

	/////////////////////////////////////////
	//bool showInExplorer();
	bool openFile();

	/////////////////////////////////////////
	QString fileName();

	/////////////////////////////////////////
	QString folderPath();

	/////////////////////////////////////////
	void updateSize();

	/////////////////////////////////////////
	bool rename();

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



