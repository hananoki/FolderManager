#pragma once

#include <QMap>
//#include "Enums.h"
#include "IConfigChanged.h"

using TMap = QMap<QString, QString>;

#define CONFIG_VALUE(type,editorName) type editorName;
#define CONFIG_VALUE_INIT(type,editorName,ini) type editorName = ini;

#define CONFIG_VALUE_LIST \
	CONFIG_VALUE( QPoint, pos ) \
	CONFIG_VALUE( QSize, size ) \
	CONFIG_VALUE( QPoint, transPos ) \
	CONFIG_VALUE( QSize, transSize ) \
	CONFIG_VALUE( QString, driveLetter ) \
	CONFIG_VALUE( QString, treeWidgetHeaderSize ) \
	CONFIG_VALUE( QString, splitterSize ) \
	CONFIG_VALUE( int, tabIndex )

//CONFIG_VALUE( QString, addDataPath ) \

class Config : public QObject, public IConfigChanged {
	Q_OBJECT
public:
	CONFIG_VALUE_LIST

	static void save();
	static void load();

	virtual void changedValue() override;

};

extern Config config;
