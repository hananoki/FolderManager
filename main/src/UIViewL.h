#pragma once

#include <QWidget>

class ItemL;


//////////////////////////////////////////////////////////////////////////////////
class UIViewL : public QWidget {
	Q_OBJECT

public:
	explicit UIViewL( QWidget* parent = nullptr );
	~UIViewL();

	//void changeDrive( const QString& driveName );

signals:
	void itemSelectionChanged( ItemL* item );
	

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

