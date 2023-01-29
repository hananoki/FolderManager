#include "UIMainWindow.h"
#include "ui_UIMainWindow.h"

#include "utils.h"

#include "ItemL.h"
#include "ItemR.h"
#include "FileDB.h"
#include "ContextMenu.h"

//////////////////////////////////////////////////////////////////////////////////
class UIMainWindow::Impl : public Ui_UIMainWindow {
public:
	using self_t = UIMainWindow;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		UIMainWindow::instance = self;
		setupUi( self );

		connect(
			self,
			&self_t::signal_addItem,
			self,
			std::bind( &Impl::addItem, this, std::placeholders::_1, std::placeholders::_2 ) );
	}


	/////////////////////////////////////////
	void setup() {
		rollbackWindow();

#if NDEBUG
		toolBar->removeAction(actionDebug);
#endif

		progressBar->hide();

		viewL->setDrive( config.driveLetter + ":/" );

		$Action::triggered( action_2, std::bind( &Impl::_calcDrive, this ) );

		$Action::triggered( actionDebug, std::bind( &Impl::_actionDebug, this ) );

		

		connect( &fileDB, &FileDB::startAnalize, std::bind( &Impl::_startAnalize, this ) );
		connect( &fileDB, &FileDB::completeAnalize, std::bind( &Impl::_completeAnalize, this ) );
	}


	


	/////////////////////////////////////////
	void _startAnalize() {
		progressBar->show();
	}


	/////////////////////////////////////////
	void _completeAnalize() {
		UIStatusBar::info( u8"フォルダ容量を計算が完了しました" );
		progressBar->hide();
	}


	/////////////////////////////////////////
	void _actionDebug() {
		//for( auto& f : fs::getFiles( environment::currentDirectory(), "*.cache", SearchOption::TopDirectoryOnly ) ) {
		//	FileHash ab;
		//	binarySerializer::read( f, [&ab]( QDataStream& in ) {
		//		in >> ab;
		//	} );

		//	QStringList aa;
		//	for( auto& k : ab.keys() ) {
		//		aa << $$( "%1\t%2" ).arg( k ).arg( ab[ k ] );
		//	}
		//	fs::writeAllLines( f + ".txt", aa );
		//}
		fileDB.save();
	}


	/////////////////////////////////////////
	void _calcDrive() {
		auto button = QMessageBox::question( self, u8"確認", config.driveLetter + u8"ドライブのフォルダ容量を計算します。\nよろしいですか？" );
		if( button == QMessageBox::No )return;

		fileDB.analize( config.driveLetter );
	}


	/////////////////////////////////////////
	void addItem( QTreeWidgetItem* p1, QTreeWidgetItem* p2 ) {
		p1->addChild( p2 );
	}


	////////////////////////
	void backupWindow() {
		config.pos = self->pos();
		config.size = self->size();

		$::saveState( config.splitterSize, splitter );
	}


	/////////////////////////////////////////
	void rollbackWindow() {
		self->move( config.pos );
		self->resize( config.size );

		$::restoreState( splitter, config.splitterSize );
	}


};

UIMainWindow* UIMainWindow::instance;


//////////////////////////////////////////////////////////////////////////////////
UIMainWindow::UIMainWindow( QWidget* parent ) :
	QMainWindow( parent ),
	impl( new Impl( this ) ) {

}


/////////////////////////////////////////
UIMainWindow::~UIMainWindow() {
}


/////////////////////////////////////////
void UIMainWindow::start() {
	impl->setup();

	emit signal_start();

	//fileDB.initMO2( utils::installGamePath(), config.getMO2Path(), config.seleectMO2ProfileName );
	//UIMainWindow::changePanel( EPanelMode::Files );

}

/////////////////////////////////////////
void UIMainWindow::closeEvent( QCloseEvent* /*event*/ ) {
	emit signal_closeWindow();
	impl->backupWindow();
}


/////////////////////////////////////////
UIStatusBar* UIMainWindow::statusBar() {
	return impl->statusBar;
}

/////////////////////////////////////////
UIViewL* UIMainWindow::uiViewL() {
	return impl->viewL;
}

/////////////////////////////////////////
UIViewR* UIMainWindow::uiViewR() {
	return impl->viewR;
}
