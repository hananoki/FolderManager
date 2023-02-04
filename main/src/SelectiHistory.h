#pragma once

//////////////////////////////////////////////////////////////////////////////////
class SelectiHistory : public QObject {
	Q_OBJECT
public:
	
	SelectiHistory();

	void push( const QString& s );

signals:
	void historyUpdated();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

extern SelectiHistory selectiHistory;
