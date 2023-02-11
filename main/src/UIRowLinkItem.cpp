#include "UIRowLinkItem.h"
#include "ui_UIRowLinkItem.h"

#include "UIMainWindow.h"
#include "UIViewL.h"

//////////////////////////////////////////////////////////////////////////////////
class UIRowLinkItem::Impl : public Ui_UIRowLinkItem {
public:
	using self_t = UIRowLinkItem;
	self_t* self;

	QMenu* contextMenu;

	/////////////////////////////////////////
	Impl( self_t* _self, const QStringList& targetList ) :
		self( _self ) {

		setupUi( _self );

		contextMenu = new QMenu();

		for( auto& path : targetList ) {
			contextMenu->addAction( $Action::create( path, icon::get( path ), [path]() {
				qtWindow->selectPath( path );
			} ) );
		}

		$PushButton::click( pushButton, [&]() {
			auto r = pushButton->pos();
			r.setX( 0 );
			r.setY( 20 );
			auto r2 = pushButton->mapToGlobal( r );

			contextMenu->exec( r2 );
			pushButton->hoverLeave();
		} );

	}

};






//////////////////////////////////////////////////////////////////////////////////
UIRowLinkItem::UIRowLinkItem( QWidget* parent, const QStringList& targetList ) :
	QWidget( parent ),
	impl( new Impl( this, targetList ) ) {

}


/////////////////////////////////////////
UIRowLinkItem::~UIRowLinkItem() {
}



