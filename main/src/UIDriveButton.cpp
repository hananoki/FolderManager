#include "UIDriveButton.h"
#include "ui_UIDriveButton.h"

#include "UIMainWindow.h"
#include "UIViewL.h"
#include "ItemL.h"

using namespace cpplinq;

//////////////////////////////////////////////////////////////////////////////////
class UIDriveButton::Impl : public Ui_UIDriveButton {
public:
	using self_t = UIDriveButton;
	self_t* self;

	QHash<QChar, HFlatPushButton*> buttons;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

#ifdef NDEBUG
		pushButton_2->hide();
#endif


		pushButton->setIcon( icon::SP_TitleBarMenuButton() );
		pushButton_2->setIcon( icon::SP_ComputerIcon() );

		auto driveList = from( QDir::drives() )
			>> select( []( auto p ) {return p.absoluteFilePath(); } )
			>> to_qlist();

		// ドライブ一覧を取得してボタンを生成する
		for( auto it = driveList.rbegin(), ed = driveList.rend(); it != ed; it++ ) {
			auto* p = new  HFlatPushButton( self );
			p->setCheckable( true );
			p->setMaximumWidth( 40 );
			p->setMinimumWidth( 40 );
			auto driveLetter = ( *it )[ 0 ];
			buttons.insert( driveLetter, p );
			p->setText( $$( driveLetter ) );
			p->setIcon( icon::get( *it ) );
			horizontalLayout->insertWidget( 0, p );

			$PushButton::clicked( p, [this, p]( bool b ) {
				config.driveLetter = p->text()[ 0 ];
				updateChecked( config.driveLetter );
				emit qtWindow->uiViewL_selectPath( $$( "%1:/" ).arg( config.driveLetter ) );
			} );
		}

		// 保存されているドライブレター文字と同じボタンを探す
		// 無い場合（外部ディスク等で取り外されている）はデフォルトとしてCを割り当て直す
		auto it = buttons.find( config.driveLetter );
		if( it == buttons.constEnd() ) {
			config.driveLetter = 'C';
			it = buttons.find( config.driveLetter );
		}
	}


	/////////////////////////////////////////
	void setup() {
		updateChecked( config.driveLetter );

		$PushButton::click( pushButton, [&]() {
			QMessageBox::aboutQt( self, tr( u8"Qtについて" ) );
		} );

		/// 指定したアイテムにフォーカス
		connect(
			qtWindow,
			&UIMainWindow::uiViewL_focusItem,
			self,
			std::bind( &Impl::uiViewL_focusItem, this, std::placeholders::_1 ) );

		emit qtWindow->uiViewL_selectPath( $$( "%1:/" ).arg( config.driveLetter ) );
	}


	/////////////////////////////////////////
	/// @brief  ドライブレター文字を指定してボタンのチェック状態を更新する
	void updateChecked( QChar driveLetter ) {
		for( auto& p : buttons.keys() ) {
			buttons[ p ]->setChecked( p == driveLetter );
		}
	}


	/////////////////////////////////////////
	/// 指定したアイテムにフォーカス
	void uiViewL_focusItem( ItemL* item ) {
		updateChecked( item->fullPath[ 0 ] );
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
