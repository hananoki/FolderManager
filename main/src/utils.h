#pragma once

class ItemModFormID;
class ContextMenu;

namespace utils {
	QString mimeType( const QString& filePath );


	/////////////////////////////////////////
	void createContextMenu( std::unique_ptr<ContextMenu>* contextMenu, HTreeWidget* treeWidget );

	/////////////////////////////////////////
	void createFolderContextMenu( std::unique_ptr<ContextMenu>* contextMenu, HTreeWidget* treeWidget );



	QStringList& expandTree();
};


