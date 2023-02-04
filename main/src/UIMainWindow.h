#pragma once

#include <QMainWindow>

#define qtWindow (UIMainWindow::instance)

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

signals:
	void signal_start();
	void signal_startAfter();
	void signal_closeWindow();

	/// 親アイテムに移動
	void action_moveParent();

	/// フォルダ容量を計算
	void action_calcFolder();

	void deleteFolder( const QString& fullPath );

	/// ドライブの切り替え
	void changeDrive( QChar driveName );

	void setBrowseMode( int );

	void uiViewL_addChild( ItemL*, ItemL*, bool bCurrent = false );
	void uiViewL_sortItem();
	void uiViewL_selectPath( QString path );

	

	/// 指定したアイテムにフォーカス
	void uiViewL_focusItem( ItemL* );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};
