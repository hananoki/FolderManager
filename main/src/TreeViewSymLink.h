#pragma once

#include "HTreeWidget.h"

//////////////////////////////////////////////////////////////////////////////////
class TreeViewSymLink : public HTreeWidget {
	Q_OBJECT
public:
	explicit TreeViewSymLink( QWidget* parent = nullptr );

	/////////////////////////////////////////
	void addTop( QTreeWidgetItem* item );
	void setHidden( QTreeWidgetItem* item );

signals:
	void signal_addTop( QTreeWidgetItem* item );
	void signal_setHidden( QTreeWidgetItem* item );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

