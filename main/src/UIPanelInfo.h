#pragma once

#include <QDialog>

//////////////////////////////////////////////////////////////////////////////////
class UIPanelInfo : public QDialog {
	Q_OBJECT

public:
	explicit UIPanelInfo( QWidget* parent = nullptr );
	~UIPanelInfo();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

