#pragma once

class HTreeWidget;


//////////////////////////////////////////////////////////////////////////////////
class ActionOpenFile : public QAction {
public:
	ActionOpenFile( HTreeWidget* tw );
	static ActionOpenFile* create( HTreeWidget* p ) { return new ActionOpenFile( p ); }
};


//////////////////////////////////////////////////////////////////////////////////
class ActionShowInExplorer : public QAction {
public:
	ActionShowInExplorer( HTreeWidget* tw );
	static ActionShowInExplorer* create( HTreeWidget* p ) { return new ActionShowInExplorer( p ); }

	virtual ~ActionShowInExplorer();
	HTreeWidget* treeWidget;
};


//////////////////////////////////////////////////////////////////////////////////
class ActionCopyFileName : public QAction {
public:
	ActionCopyFileName( HTreeWidget* tw );
	static ActionCopyFileName* create( HTreeWidget* p ) { return new ActionCopyFileName( p ); }

	HTreeWidget* treeWidget;
};


//////////////////////////////////////////////////////////////////////////////////
class ActionDelete : public QAction {
public:
	ActionDelete( HTreeWidget* tw );
	static ActionDelete* create( HTreeWidget* p ) { return new ActionDelete( p ); }

	HTreeWidget* treeWidget;
};


//////////////////////////////////////////////////////////////////////////////////
class ActionRename : public QAction {
public:
	ActionRename( HTreeWidget* tw );
	static ActionRename* create( HTreeWidget* p ) { return new ActionRename( p ); }

	HTreeWidget* treeWidget;
};


//////////////////////////////////////////////////////////////////////////////////
class ActionShowProperty : public QAction {
public:
	ActionShowProperty( HTreeWidget* tw );
	static ActionShowProperty* create( HTreeWidget* p ) { return new ActionShowProperty( p ); }

	HTreeWidget* treeWidget;
};


//////////////////////////////////////////////////////////////////////////////////
class Action_Analize : public QAction {
public:
	Action_Analize( HTreeWidget* tw );
	static Action_Analize* create( HTreeWidget* p ) { return new Action_Analize( p ); }
};

//////////////////////////////////////////////////////////////////////////////////
class Action_RemoveFolder : public QAction {
public:
	Action_RemoveFolder( HTreeWidget* tw );
	static Action_RemoveFolder* create( HTreeWidget* p );
};

