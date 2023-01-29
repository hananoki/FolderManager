#pragma once

// フォルダーのフルパス、サイズ
using FileHash = QHash<QString, qint64>;

struct DriveCache final {
public:
	FileHash folderSize;

	QHash<QString, QString> symbolicLink;

	static void read( DriveCache& dc, const QString& filePath );

	static void write( DriveCache& dc, QChar driveName );

	void set( const QString& fullPath, qint64 size );
	void setSymbolicLink( const QString& fullPath, const QString& targetPath );

	qint64 get( const QString& fullPath );
};

