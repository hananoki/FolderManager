#include "UIDriveButton.h"
#include "ui_UIDriveButton.h"

#include "UIMainWindow.h"

using namespace cpplinq;

//////////////////////////////////////////////////////////////////////////////////
class UIDriveButton::Impl : public Ui_UIDriveButton {
public:
	using self_t = UIDriveButton;
	self_t* self;

	QHash<QString, HFlatPushButton*> buttons;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

		auto driveList = from( QDir::drives() )
			>> select( []( auto p ) {return p.absoluteFilePath(); } )
			>> to_qlist();

		for( auto it = driveList.rbegin(); it != driveList.rend(); it++ ) {
			auto* p = new  HFlatPushButton( self );
			p->setCheckable( true );
			auto driveLetter = $$( ( *it )[ 0 ] );
			buttons.insert( driveLetter, p );
			p->setText( driveLetter );
			p->setIcon( icon::get( *it ) );
			verticalLayout->insertWidget( 0, p );

			$PushButton::clicked( p, [this, p]( bool b ) {
				for( auto& pp : buttons ) {
					pp->setChecked( false );
				}
				p->setChecked( true );
				config.driveLetter = p->text();
				emit qtWindow->signal_selectDrive( p->text()[ 0 ] );
			} );
		}
	}

	/////////////////////////////////////////
	void setup() {
		

		auto it = buttons.find( config.driveLetter );
		if( it == buttons.constEnd() ) {
			config.driveLetter = "C";
			it = buttons.find( config.driveLetter );
		}
		( *it )->setChecked( true );
		emit qtWindow->signal_selectDrive( ( *it )->text()[ 0 ] );
	}

};


//////////////////////////////////////////////////////////////////////////////////
UIDriveButton::UIDriveButton( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, [&]() {
		impl->setup();
	} );
}

UIDriveButton::~UIDriveButton() {
}
