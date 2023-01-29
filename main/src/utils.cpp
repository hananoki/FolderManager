#include "utils.h"

#include "ItemR.h"
#include "ItemL.h"
#include "ActionFile.h"
//#include "ActionFolder.h"
#include "ContextMenu.h"

namespace utils {

	QMimeDatabase mimeDatabase;
	QHash<QString, QString> mimeSuffix;

	/////////////////////////////////////////
	QString mimeType( const QString& filePath ) {
		auto suffix = QFileInfo( filePath ).suffix();
		//auto suffix = path::getSuffix( filePath );
		auto it = mimeSuffix.find( suffix );
		if( it == mimeSuffix.constEnd() ) {
			auto m = mimeDatabase.mimeTypeForFile( filePath );
			//auto m = mimeDatabase.mimeTypeForName( m2 .iconName());
			mimeSuffix.insert( suffix, m.comment() );
			it = mimeSuffix.find( suffix );
			qDebug() << m.name();
			qDebug() << m.comment();
			qDebug() << m.genericIconName();
			qDebug() << m.iconName();
			qDebug() << m.filterString();
		}

		return ( *it );
	}

	/////////////////////////////////////////
	void createContextMenu( std::unique_ptr<ContextMenu>* contextMenu, HTreeWidget* treeWidget ) {
		treeWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		$TreeWidget::customContextMenuRequested( treeWidget, [treeWidget, contextMenu]( const QPoint& pos ) {
			QModelIndex index = treeWidget->indexAt( pos );
			if( !index.isValid() ) return;

			auto* itemBase = reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() );

			auto* itemFileInfo = dynamic_cast<ItemFileInfo*>( itemBase );

			( *contextMenu ) = std::make_unique<ContextMenu>();

			if( itemFileInfo ) {
				( *contextMenu )->addAction<ActionOpenFile>( treeWidget );
				( *contextMenu )->addAction<ActionShowInExplorer>( treeWidget );
				( *contextMenu )->addAction<ActionCopyFileName>( treeWidget );
				( *contextMenu )->addSeparator();
				( *contextMenu )->addAction<ActionDelete>( treeWidget );
				( *contextMenu )->addSeparator();
				( *contextMenu )->addAction<ActionShowProperty>( treeWidget );
			}

			( *contextMenu )->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		} );
	}

#if false
	/////////////////////////////////////////
	void createFolderContextMenu( std::unique_ptr<ContextMenu>* contextMenu, HTreeWidget* treeWidget ) {
		$TreeWidget::customContextMenuRequested( treeWidget, [treeWidget, contextMenu]( const QPoint& pos ) {
			QModelIndex index = treeWidget->indexAt( pos );
			if( !index.isValid() ) return;

			auto* item = dynamic_cast<ItemFolder*>( reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() ) );
			if( item ) {
				( *contextMenu ) = std::make_unique<ContextMenu>();
				( *contextMenu )->addAction<ActionOpenFolder>( treeWidget );
				( *contextMenu )->addAction<ActionFolderPathCopy>( treeWidget );

				auto* item2 = dynamic_cast<ItemFolderMO2View*>( item );
				if( item2 ) {
					( *contextMenu )->addSeparator();
					( *contextMenu )->addAction<ActionFolderOpenNexus>( treeWidget );
			}

				( *contextMenu )->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		}

	} );
}
#endif

};


