#include "utils.h"

#include "ItemR.h"
#include "ItemL.h"
#include "ItemSymLink.h"
#include "ActionFile.h"

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

		$TreeWidget::customContextMenuRequested( treeWidget, [contextMenu,treeWidget]( const QPoint& pos ) {
			QModelIndex index = treeWidget->indexAt( pos );
			if( !index.isValid() ) return;

			auto* itemBase = reinterpret_cast<QTreeWidgetItem*>( index.internalPointer() );

			auto* itemFileInfo = dynamic_cast<ItemFileInfo*>( itemBase );
			auto* itemSymLink = dynamic_cast<ItemSymLink*>( itemBase );

			( *contextMenu ) = std::make_unique<ContextMenu>();

			if( itemFileInfo ) {
				if( itemFileInfo->isFile() ) {
					( *contextMenu )->addAction<ActionOpenFile>( treeWidget );
				}
				( *contextMenu )->addAction<ActionShowInExplorer>( treeWidget );
				( *contextMenu )->addSeparator();
				( *contextMenu )->addAction<ActionCopyPathName>( treeWidget );
				( *contextMenu )->addAction<ActionCopyFullPathName>( treeWidget );
				if( itemSymLink ) {
					( *contextMenu )->addAction<Action_ShowTargetPath>( treeWidget );
				}
#ifdef Q_OS_WIN
				( *contextMenu )->addSeparator();
				auto lst = QStandardPaths::standardLocations( QStandardPaths::ApplicationsLocation );
				auto w = path::getDirectoryName( path::getDirectoryName( lst[ 0 ] ) );
				w += "/SendTo";
				for( auto& f : fs::getFiles( w, "*.lnk" ) ) {
					QFileInfo fi( f );
					auto ss = fi.symLinkTarget();
					( *contextMenu )->addAction<Action_LnkFile>( treeWidget, ss );
				}
#endif
				( *contextMenu )->addSeparator();
				( *contextMenu )->addAction<ActionDelete>( treeWidget );
				( *contextMenu )->addAction<ActionRename>( treeWidget );
				( *contextMenu )->addSeparator();
				( *contextMenu )->addAction<ActionShowProperty>( treeWidget );
			}

			( *contextMenu )->exec( treeWidget->viewport()->mapToGlobal( pos ) );
		} );
	}

};


