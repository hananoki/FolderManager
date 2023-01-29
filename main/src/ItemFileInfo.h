#pragma once

//////////////////////////////////////////////////////////////////////////////////
class ItemFileInfo : public HTreeWidgetItem {
public:
	QString fullPath;
	QFileInfo fileInfo;

	/////////////////////////////////////////
	ItemFileInfo( QTreeWidget* parent, const QString& _fullPath );

	/////////////////////////////////////////
	bool isFolder();
	bool isFile();
	bool isRoot();

	/////////////////////////////////////////
	bool openFile();

	/////////////////////////////////////////
	bool showInExplorer();
};


/////////////////////////////////////////
inline
bool ItemFileInfo::isFolder() {
	return fileInfo.isDir();
}

/////////////////////////////////////////
inline
bool ItemFileInfo::isFile() {
	return fileInfo.isFile();
}

/////////////////////////////////////////
inline
bool ItemFileInfo::isRoot() {
	return fileInfo.isRoot();
}