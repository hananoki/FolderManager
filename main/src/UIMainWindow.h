#pragma once

#include <QMainWindow>

#define qtWindow (UIMainWindow::instance)
#define qtWindowUiViewL (qtWindow->uiViewL())
#define qtWindowUiViewR (qtWindow->uiViewR())

class UIViewL;
class UIViewR;
class UIStatusBar;
class ItemL;

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

	void selectPath( QString path );

	/// ドライブの切り替え
	void changeDrive( QChar driveName );

	/// フォルダ容量を計算
	void calcFolder();

signals:
	void signal_start();
	void signal_startAfter();
	void signal_closeWindow();

	/// 親アイテムに移動
	void signal_moveParent();

	/// フォルダ容量を計算
	void signal_calcFolder();

	void deleteItem( const QString& fullPath );

	void signal_selectPath( const QString& path );
	void signal_changeDrive( QChar driveName );

	void setBrowseMode( int );

	
	void uiViewL_sortItem();
	

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};
