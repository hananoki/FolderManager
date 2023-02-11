#pragma once

//////////////////////////////////////////////////////////////////////////////////
struct SizeSet final {
	qint64 folderFileSize;
	qint64 fileSize;
};

// フォルダーのフルパス、サイズ
using FileHash = QHash<QString, SizeSet>;

//////////////////////////////////////////////////////////////////////////////////
struct DriveCache final {
public:
	FileHash folderFileSize;

	QHash<QString, QString> symbolicLink;

	static void read( DriveCache& dc, const QString& filePath );

	static void write( DriveCache& dc, QChar driveName );

	void set( const QString& fullPath, qint64 folderSize, qint64 fileSize );
	void setSymbolicLink( const QString& fullPath, const QString& targetPath );

	SizeSet get( const QString& fullPath );

	bool isEmpty();
};

