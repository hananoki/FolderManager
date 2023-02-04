﻿#pragma once

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
	static ActionDelete* create( HTreeWidget* p );

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
class Action_ShowTargetPath : public QAction {
public:
	Action_ShowTargetPath( HTreeWidget* tw );
	static Action_ShowTargetPath* create( HTreeWidget* p ) { return new Action_ShowTargetPath( p ); }
};


//////////////////////////////////////////////////////////////////////////////////
class Action_LnkFile : public QAction {
public:
	QString filePath;
	Action_LnkFile( HTreeWidget* tw, const QString& filePath );
	static Action_LnkFile* create( HTreeWidget* p, const QString& filePath ) { return new Action_LnkFile( p, filePath ); }
};
