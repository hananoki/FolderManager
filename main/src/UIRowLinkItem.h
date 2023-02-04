#pragma once

#include <QWidget>

//////////////////////////////////////////////////////////////////////////////////
class UIRowLinkItem : public QWidget {
	Q_OBJECT

public:
	explicit UIRowLinkItem( QWidget* parent, const QStringList& targetList );
	~UIRowLinkItem();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

