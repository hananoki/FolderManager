#pragma once

#include <QApplication>

#define qtApp (App::instance)

//////////////////////////////////////////////////////////////////////////////////
class App : public QApplication {
	Q_OBJECT

public:
	static App* instance;

	App( int& argc, char** argv, int = ApplicationFlags );
	virtual ~App();

signals:
	

public:
	class Impl;
	std::unique_ptr<Impl> impl;
};

