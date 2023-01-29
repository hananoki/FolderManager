#pragma once

#include <QMainWindow>

#define qtWindow (UIMainWindow::instance)

class UIViewL;
class UIViewR;
class UIStatusBar;

//////////////////////////////////////////////////////////////////////////////////
class UIMainWindow : public QMainWindow {
	Q_OBJECT

public:
	UIMainWindow( QWidget* parent = nullptr );
	~UIMainWindow();

	static UIMainWindow* instance;

	/////////////////////////////////////////
	void start();

	/////////////////////////////////////////
	virtual void closeEvent( QCloseEvent* event ) override;

	UIStatusBar* statusBar();
	UIViewL* uiViewL();
	UIViewR* uiViewR();

signals:
	void signal_start();
	void signal_closeWindow();

	// ƒAƒCƒeƒ€’Ç‰Á
	void signal_addItem( QTreeWidgetItem*, QTreeWidgetItem* );
	void signal_selectDrive( QChar driveName );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};
