#pragma once

#include "HTreeWidget.h"

//////////////////////////////////////////////////////////////////////////////////
class TreeViewFileBrowse : public HTreeWidget {
	Q_OBJECT
public:
	explicit TreeViewFileBrowse( QWidget* parent = nullptr );

	/// 新しいパスに移動
	//void selectPath( const QString& path );

	void addTop( QTreeWidgetItem* item );

	void mouseMoveEvent( QMouseEvent* e );
	void mouseReleaseEvent( QMouseEvent* e );

signals:
	void signal_addTop( QTreeWidgetItem* item );
	void setHidden( QTreeWidgetItem* item );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

