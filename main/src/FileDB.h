#pragma once

#include "DriveCache.h"

class FileDB : public QObject {
	Q_OBJECT
public:
	FileDB();

	void init();

	void setSize( const QString& fullPath, qint64 folderSize, qint64 fileSize );
	void setSymbolicLink( const QString& fullPath, const QString& targetPath );
	qint64 get( const QString& fullPath );
	SizeSet getSizeSet( const QString& fullPath );
	void analize( const QString& driveName );

	void save();

	DriveCache& driveCache( QChar drive );

	QStringList symbolicLinkSource( const QString& fullPath );

signals:
	void startAnalize();
	void completeAnalize();

	void completeFileLoad();

	void errorCache( QString path );

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

extern FileDB fileDB;


