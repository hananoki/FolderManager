#include "UIMainWindow.h"
#include "ui_UIMainWindow.h"

#include "utils.h"

#include "ItemL.h"
#include "ItemR.h"
#include "FileDB.h"
#include "ContextMenu.h"
#include "revinfo.inc"

//////////////////////////////////////////////////////////////////////////////////
class UIMainWindow::Impl : public Ui_UIMainWindow {
public:
	using self_t = UIMainWindow;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	QList<QAction*> browseActions;

	QStringList errorList;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		UIMainWindow::instance = self;
		setupUi( self );

		connect( &fileDB, &FileDB::signal_errorCache, self, std::bind( &Impl::errorCache, this, std::placeholders::_1 ) );
		connect( &fileDB, &FileDB::signal_startAnalize, std::bind( &Impl::startAnalize, this ) );
		connect( &fileDB, &FileDB::signal_completeAnalize, std::bind( &Impl::completeAnalize, this ) );
		connect( &fileDB, &FileDB::signal_startFileLoad, std::bind( &Impl::startFileLoad, this ) );
		connect( &fileDB, &FileDB::signal_completeFileLoad, std::bind( &Impl::completeFileLoad, this ) );


		fileDB.init();

		//self->setWindowTitle( $$( "%1 rev %2" ).arg( self->windowTitle() ).arg( revno ) );
	}


	/////////////////////////////////////////
	void setup() {
		rollbackWindow();

#ifdef NDEBUG
		toolBar->removeAction( actionDebug );
#endif

		progressBar->hide();
		actionL->setEnabled( false );
		actionR->setEnabled( false );


		action_fileView->setCheckable( true );
		action_symbolicLink->setCheckable( true );
		browseActions << action_fileView;
		browseActions << action_symbolicLink;

		action_fileView->setChecked( true );

		/// 親アイテムに移動
		$Action::triggered( action_moveParent, std::bind( &self_t::signal_moveParent, self ) );

		/// フォルダ容量を計算
		$Action::triggered( action_calcFolder, std::bind( &self_t::calcFolder, self ) );

		$Action::triggered( actionDebug, std::bind( &Impl::_actionDebug, this ) );


		$Action::triggered( action_fileView, [&]() {
			setBrowseMode( action_fileView );
		} );
		$Action::triggered( action_symbolicLink, [&]() {
			setBrowseMode( action_symbolicLink );
		} );


		// 左ビューの選択
		connect(
			viewL,
			&UIViewL::signal_itemSelectionChanged,
			std::bind( &Impl::uiViewL_itemSelectionChanged, this, std::placeholders::_1 ) );

	}





	/////////////////////////////////////////
	void uiViewL_selectPath( QString path ) {
		QFileInfo fi( path );
		action_moveParent->setEnabled( !fi.isRoot() );
	}


	/////////////////////////////////////////
	void setBrowseMode( QAction* act ) {
		int i = 0;
		for( auto& p : browseActions ) {
			bool b = p == act;
			p->setChecked( b );

			if( b ) {
				emit self->setBrowseMode( i );
			}
			i++;
		}
	}


	/////////////////////////////////////////
	void startAnalize() {
		progressBar->show();
	}


	/////////////////////////////////////////
	void completeAnalize() {
		UIStatusBar::info( u8"フォルダ容量を計算が完了しました" );
		progressBar->hide();
	}


	/////////////////////////////////////////
	void startFileLoad() {
		UIStatusBar::progressStart( u8"キャッシュデータを読み込みんでいます " );
	}


	/////////////////////////////////////////
	void completeFileLoad() {
		UIStatusBar::progressStop();
		if( errorList.isEmpty() ) {
			UIStatusBar::info( u8"キャッシュデータを読み込みました" );
		}
		else {
			QMessageBox::warning( self, u8"情報", u8"キャッシュデータが壊れているため、読み込みを中止しました\n\n" + errorList.join( "\n" ) );
			UIStatusBar::warning( u8"キャッシュデータが壊れているため、読み込みを中止しました" );
		}
	}


	/////////////////////////////////////////
	void errorCache( QString path ) {
		errorList << path;
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
		//fileDB.save();
	}


	/////////////////////////////////////////
	void uiViewL_itemSelectionChanged( ItemL* item ) {
		setBrowseMode( action_fileView );
	}


	/////////////////////////////////////////
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
	//emit signal_startAfter();

	selectPath( $$( "%1:/" ).arg( config.driveLetter ) );
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

/////////////////////////////////////////
void UIMainWindow::selectPath( QString path ) {
	emit qtWindow->signal_selectPath( path );
}

/////////////////////////////////////////
void UIMainWindow::changeDrive( QChar driveName ) {
	emit qtWindow->signal_changeDrive( driveName );
}

/////////////////////////////////////////
/// フォルダ容量を計算
void UIMainWindow::calcFolder() {
	emit qtWindow->signal_calcFolder();
}
