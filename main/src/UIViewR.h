#pragma once

#include <QWidget>

class ItemL;
class ItemR;

//////////////////////////////////////////////////////////////////////////////////
class UIViewR : public QWidget {
	Q_OBJECT

public:
	explicit UIViewR( QWidget* parent = nullptr );
	~UIViewR();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

