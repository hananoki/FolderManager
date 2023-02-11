#pragma once

#include <QWidget>

class ItemL;


//////////////////////////////////////////////////////////////////////////////////
class UIViewL : public QWidget {
	Q_OBJECT
		friend class ItemL;

public:
	explicit UIViewL( QWidget* parent = nullptr );
	~UIViewL();

	//bool isBusy();

	void addTop( ItemL* );
	void addChild( ItemL*, ItemL*, bool bCurrent = false );
	void setHidden( ItemL*, bool flg );

	/// 新しいパスに移動
	//void selectPath( const QString& path );
	void focusItem( ItemL* item, bool changedrive );

signals:
	void signal_itemSelectionChanged( ItemL* item );

	void signal_addTop( ItemL* );
	void signal_addChild( ItemL*, ItemL*, bool bCurrent = false );
	void signal_setHidden( ItemL*, bool flg );

	//void signal_changedPath( QString path );

	/// 指定したアイテムにフォーカス
	void signal_focusItem( ItemL* item, bool changedrive );

private:
	class Impl;
	std::unique_ptr<Impl> impl;

	void setCache( const QString& path, ItemL* item );
	ItemL* getCached( const QString& path );
};

