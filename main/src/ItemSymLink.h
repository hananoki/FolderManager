#pragma once

#include "ItemFileInfo.h"

//////////////////////////////////////////////////////////////////////////////////
class ItemSymLink : public ItemFileInfo {
public:
	ItemSymLink( HTreeWidget* parent, const QString& _fullPath, const QString& _targegtPath );

	QString targegtPath;
};
