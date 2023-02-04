#include "App.h"
#include "UIMainWindow.h"
#include "FileDB.h"

//////////////////////////////////////////////////////////////////////////////////
class App::Impl {
public:
	App* self;
	

	///////////////////////////////////////////
	Impl( App* _self )
		: self( _self ) {

		
	}


	/////////////////////////////////////////
	~Impl() {
	}

	
	


	/////////////////////////////////////////
	bool loadCurrentLangTranslator( QTranslator& translator ) {
		QString current_langfile = "qt_ja";
		if( fs::isExistFile( "translations/qt_ja.qm" ) ) {
			if( !translator.load( current_langfile, "translations" ) ) {
				assert( false );
				return false;
			}
		}
		return true;
	}


	/////////////////////////////////////////
	int run() {

		QTranslator t;
		loadCurrentLangTranslator( t );
		self->installTranslator( &t );

		UIMainWindow w;

		QKeySequence keys_refresh( QKeySequence::Refresh );
		auto* shortcutF5 = new QAction( &w );
		shortcutF5->setShortcut( keys_refresh );
		w.addAction( shortcutF5 );
		connect( shortcutF5, &QAction::triggered, []() {
			qApp->setStyleSheet( fs::readAllText( "main/src/style.qss" ) );
			HLogView::info( tr( u8"スタイルシートを更新" ) );
		} );

		w.start();

		qApp->setStyleSheet( fs::readAllText( ":/res/style.qss" ) );

		w.show();

		return self->exec();
	}

};

App* App::instance;

//////////////////////////////////////////////////////////////////////////////////
App::App( int& argc, char** argv, int /*= ApplicationFlags*/ ) :
	QApplication( argc, argv ),
	impl( new Impl( this ) ) {

	instance = this;
	config.load();
}


/////////////////////////////////////////
App::~App() {
	config.save();
}


//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg };
void logHandler( QtMsgType type, const QMessageLogContext& context, const QString& msg ) {
	const auto& message = qFormatLogMessage( type, context, msg );
	//switch( type ) {
	//	default:
	//		HLogView::info( msg );
	//		break;
	//}

	//QTextStream cerr( stderr );
	//cerr << message << endl;
	//QFile file( "App.log" );
	//if( !file.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
	//	cerr << "Cannot open log file:" << file.fileName();
	//	return;
	//}
	//QTextStream( &file ) << message << endl;
}


int main( int argc, char* argv[] ) {
	qInstallMessageHandler( logHandler );
	qSetMessagePattern( "[%{time yyyy-MM-dd hh:mm:ss}] [%{category}] [%{threadid}] %{file}:%{line} => %{message} " );
	App app( argc, argv );
	return app.impl->run();
}
