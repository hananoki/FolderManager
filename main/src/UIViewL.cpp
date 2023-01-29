#include "UIViewL.h"
#include "ui_UIViewL.h"

#include "UIMainWindow.h"
#include "UIViewR.h"
#include "ItemL.h"
#include "ItemR.h"
#include "ContextMenu.h"
#include "ActionFile.h"



//////////////////////////////////////////////////////////////////////////////////
class UIViewL::Impl : public Ui_UIViewL {
public:
	using self_t = UIViewL;
	self_t* self;

	QFuture<void> future;
	QFutureWatcher<void> watcher;

	std::unique_ptr<ContextMenu> contextMenu;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {

		setupUi( self );

	}


	/////////////////////////////////////////
	void _start() {
		createContextMenu();

		treeWidget->header()->resizeSection(1,16);

		$TreeWidget::itemSelectionChanged(
			treeWidget,
			std::bind( &Impl::_itemSelectionChanged, this ) );

		connect(
			treeWidget,
			&QTreeWidget::itemExpanded,
			std::bind( &Impl::itemExpanded, this, std::placeholders::_1 ) );

		/// 右ビューのダブルクリック
		connect(
			qtWindow->uiViewR(),
			&UIViewR::itemDoubleClicked,
			std::bind( &Impl::itemDoubleClicked, this, std::placeholders::_1 ) );

		connect(
			qtWindow,
			&UIMainWindow::signal_selectDrive,
			std::bind( &Impl::_selectDrive, this, std::placeholders::_1 ) );
	}


	/////////////////////////////////////////
	void _selectDrive( QChar driveName ) {
		setDrive( $$( "%1:/" ).arg( driveName ) );
	}


	/////////////////////////////////////////
	/// アイテムの選択切り替え
	void _itemSelectionChanged() {
		auto* item = treeWidget->currentItem<ItemL>();

		self->itemSelectionChanged( item );// 
	}


	/////////////////////////////////////////
	void itemExpanded( QTreeWidgetItem* item ) {
		auto* itemL = (ItemL*) item;
		QtConcurrent::run( [itemL]() {
			for( auto& p : itemL->childItems<ItemL>() ) {
				p->makeChild();
			}
		} );
	}


	/////////////////////////////////////////
	/// 右ビューのダブルクリック
	void itemDoubleClicked( ItemR* itemR ) {
		qDebug() << itemR->fullPath;
		auto paths = itemR->fullPath.split( "/" );

		auto* item = treeWidget->findTopLevelItem<ItemL>( paths[ 0 ] + "/" );
		if( !item )return;
		paths.takeFirst();
		while( !paths.isEmpty() ) {
			item = item->find<ItemL>( paths[ 0 ] );
			if( !item )return;
			paths.takeFirst();
		}
		treeWidget->setCurrentItem( item );
	}


	/////////////////////////////////////////
	void createContextMenu() {
		treeWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		$TreeWidget::customContextMenuRequested( treeWidget, [&]( const QPoint& pos ) {
			QModelIndex index = treeWidget->indexAt( pos );
			if( !index.isValid() ) return;

			contextMenu = std::make_unique<ContextMenu>();

			auto* itemBase = reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() );

			auto* itemFileInfo = dynamic_cast<ItemFileInfo*>( itemBase );
			if( itemFileInfo ) {
				contextMenu->addAction<Action_RemoveFolder>( treeWidget );
			}

			auto* item = dynamic_cast<ItemL*>( reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() ) );
			if( item ) {

				contextMenu->addAction<Action_Analize>( treeWidget );
				//( *contextMenu )->addAction<ActionShowInExplorer>( treeWidget );
				//( *contextMenu )->addAction<ActionCopyFileName>( treeWidget );


			}
			contextMenu->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		} );
	}


	////////////////////////
	void setDrive( const QString& driveName ) {
		treeWidget->clear();

		auto* root = new ItemL( nullptr, driveName );

		treeWidget->addTopLevelItem( root );

		QtConcurrent::run( [root]() {
			root->makeChild();
		} );
	}


};


//////////////////////////////////////////////////////////////////////////////////
UIViewL::UIViewL( QWidget* parent ) :
	QWidget( parent ),
	impl( new Impl( this ) ) {

	connect( qtWindow, &UIMainWindow::signal_start, std::bind( &Impl::_start, impl.get() ) );
}


/////////////////////////////////////////
UIViewL::~UIViewL() {
}


////////////////////////
void UIViewL::setDrive( const QString& driveName ) {
	impl->setDrive( driveName );
}
