#pragma once

#include "DriveCache.h"

class FileDB : public QObject {
	Q_OBJECT
public:
	FileDB();

	//void set( const QFileInfo& finfo );
	void setSize( const QString& fullPath, qint64 size );
	void setSymbolicLink( const QString& fullPath, const QString& targetPath );
	qint64 get( const QString& fullPath );

	void analize( const QString& driveName );

	void save();

	DriveCache& driveCache( QChar drive );

	QStringList symbolicLinkSource( const QString& fullPath );

signals:
	void startAnalize();
	void completeAnalize();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

extern FileDB fileDB;


