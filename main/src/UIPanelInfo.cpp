#include "UIPanelInfo.h"
#include "ui_UIPanelInfo.h"
#include "revinfo.inc"

//////////////////////////////////////////////////////////////////////////////////
class UIPanelInfo::Impl : public Ui_UIPanelInfo {
public:

	UIPanelInfo* self;

	/////////////////////////////////////////
	Impl( UIPanelInfo* _self ) :
		self( _self ) {

		setupUi( self );

		label_rev->setText( $$( "revision: %1" ).arg( revno ) );
		
		textBrowser->setMarkdown( fs::readAllText( ":/res/markdown/about.md" ) );
	}
};


//////////////////////////////////////////////////////////////////////////////////
UIPanelInfo::UIPanelInfo( QWidget* parent ) :
	QDialog( parent ),
	impl( new Impl( this ) ) {

}


/////////////////////////////////////////
UIPanelInfo::~UIPanelInfo() {

}
