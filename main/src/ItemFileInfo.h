#pragma once

//////////////////////////////////////////////////////////////////////////////////
class ItemFileInfo : public HTreeWidgetItem {
public:
	QString fullPath;
	HFileInfo fileInfo;
	HTreeWidget* tw;

	/////////////////////////////////////////
	ItemFileInfo( HTreeWidget* parent, const QString& _fullPath );

	/////////////////////////////////////////
	bool isFolder();
	bool isFile();
	bool isRoot();
	bool isSymbolicLink();

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

/////////////////////////////////////////
inline
bool ItemFileInfo::isSymbolicLink() {
	return fileInfo.isSymbolicLink() | fileInfo.isJunction();
}