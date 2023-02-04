#pragma once

//////////////////////////////////////////////////////////////////////////////////
class ContextMenu : public QMenu {
public:
	template<class T>
	void addAction( HTreeWidget* treeWidget ) {
		auto* p = T::create( treeWidget );
		if( !p )return;
		lst << p;
		QMenu::addAction( p );
	}
	template<class T, class U>
	void addAction( HTreeWidget* treeWidget, U a ) {
		auto* p = T::create( treeWidget, a );
		if( !p )return;
		lst << p;
		QMenu::addAction( p );
	}

	virtual ~ContextMenu();
	QList<QAction*> lst;
};


