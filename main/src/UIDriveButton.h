#pragma once

#include <QWidget>

//////////////////////////////////////////////////////////////////////////////////
class UIDriveButton : public QWidget {
	Q_OBJECT

public:
	explicit UIDriveButton( QWidget* parent = nullptr );
	~UIDriveButton();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

