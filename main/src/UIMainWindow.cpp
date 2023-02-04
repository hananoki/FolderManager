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

	QList<QAction*> browseActions;

	QStringList errorList;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		UIMainWindow::instance = self;
		setupUi( self );
		//actionL->setIcon( icon::SP_ArrowBack() );
		//actionR->setIcon( icon::SP_ArrowForward() );
		connect(
			&fileDB,
			&FileDB::errorCache,
			self,
			std::bind( &Impl::errorCache, this, std::placeholders::_1 ) );

		fileDB.init();
	}


	/////////////////////////////////////////
	void setup() {
		rollbackWindow();

#ifdef NDEBUG
		toolBar->removeAction( actionDebug );
#endif

		progressBar->hide();
		actionL->setEnabled(false);
		actionR->setEnabled( false );


		action_fileView->setCheckable( true );
		action_symbolicLink->setCheckable( true );
		browseActions << action_fileView;
		browseActions << action_symbolicLink;

		action_fileView->setChecked( true );

		/// 親アイテムに移動
		$Action::triggered( action_moveParent, std::bind( &self_t::action_moveParent, self ) );

		/// フォルダ容量を計算
		$Action::triggered( action_calcFolder, std::bind( &self_t::action_calcFolder, self ) );

		$Action::triggered( actionDebug, std::bind( &Impl::_actionDebug, this ) );



		$Action::triggered( action_fileView, [&]() {
			setBrowseMode( action_fileView );
		} );
		$Action::triggered( action_symbolicLink, [&]() {
			setBrowseMode( action_symbolicLink );
		} );

		connect( &fileDB, &FileDB::startAnalize, std::bind( &Impl::_startAnalize, this ) );
		connect( &fileDB, &FileDB::completeAnalize, std::bind( &Impl::_completeAnalize, this ) );
		connect( &fileDB, &FileDB::completeFileLoad, std::bind( &Impl::completeFileLoad, this ) );

		// 左ビューの選択
		connect(
			viewL,
			&UIViewL::itemSelectionChanged,
			std::bind( &Impl::uiViewL_itemSelectionChanged, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::uiViewL_selectPath,
			self,
			std::bind( &Impl::uiViewL_selectPath, this, std::placeholders::_1 ) );
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
	void _startAnalize() {
		progressBar->show();
	}


	/////////////////////////////////////////
	void _completeAnalize() {
		UIStatusBar::info( u8"フォルダ容量を計算が完了しました" );
		progressBar->hide();
	}


	/////////////////////////////////////////
	void completeFileLoad() {
		if( errorList.isEmpty() ) {
			UIStatusBar::info( u8"キャッシュデータを読みcompleteFileLoad" );
		}
		else {
			QMessageBox::warning( self, u8"情報", u8"キャッシュデータが壊れているため、読み込みを中止しました\n\n"+ errorList.join("\n") );
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

	emit uiViewL_selectPath( $$( "%1:/" ).arg( config.driveLetter ) );
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
