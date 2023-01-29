#include "Config.h"
#include "json.h"
#include <magic_enum.hpp>

using namespace cpplinq;

//DECLARE_ENUM( nkf::ECharacterCode )
//DECLARE_ENUM( EGameTitle )

Config config;

///////////////////////////////////////
void Config::load() {
	bool bOK = json::read( QCoreApplication::applicationName() + ".json", []( const QJsonObject& jsonObj ) {
#undef CONFIG_VALUE
#define CONFIG_VALUE(type,editorName) config. editorName = json::GetValue<type>( jsonObj[ #editorName ] );

		CONFIG_VALUE_LIST
	} );

	if( !bOK ) {
		QDesktopWidget d;

		config.size = QSize( 800, 600 );
		config.pos = QPoint( ( d.width() / 2 ) - ( config.size.width() / 2 ) , ( d.height() / 2 ) - ( config.size.height() / 2 ) );
	}

	if( config.driveLetter.isEmpty() ) {
		config.driveLetter = "C";
	}
}


///////////////////////////////////////
void Config::save() {
	json::write( QCoreApplication::applicationName() + ".json", []( QJsonObject& jsonObj ) {

#undef CONFIG_VALUE
#define CONFIG_VALUE(type,editorName) json::SetValue( jsonObj , #editorName, config. editorName );

		CONFIG_VALUE_LIST
	} );
}


///////////////////////////////////////
void Config::changedValue() {
	save();
}

