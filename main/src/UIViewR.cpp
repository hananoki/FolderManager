#include "UIViewR.h"
#include "ui_UIViewR.h"

#include "UIMainWindow.h"
#include "UIViewL.h"

#include "ItemL.h"
#include "ItemR.h"
#include "utils.h"
#include "ContextMenu.h"
#include "FileDB.h"

#include "DriveCache.h"
#include "ActionFile.h"

//////////////////////////////////////////////////////////////////////////////////
class UIViewR::Impl : public Ui_UIViewR {
public:
	using self_t = UIViewR;
	self_t* self;

	QList<QWidget*> view;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

		view << treeWidget;
		view << treeWidget_2;
	}


	/////////////////////////////////////////
	void _start() {
		treeWidget_2->hide();

		connect(
			qtWindow,
			&UIMainWindow::setBrowseMode,
			std::bind( &Impl::setBrowseMode, this, std::placeholders::_1 ) );
	}


	/////////////////////////////////////////
	void setBrowseMode( int n ) {
		for( int i = 0; i < view.length(); i++ ) {
			view[ i ]->setVisible( i == n );
		}
	}

};



//////////////////////////////////////////////////////////////////////////////////
UIViewR::UIViewR( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, std::bind( &Impl::_start, impl.get() ) );

}


/////////////////////////////////////////
UIViewR::~UIViewR() {
}



