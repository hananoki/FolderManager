#include "SelectiHistory.h"

SelectiHistory selectiHistory;

//////////////////////////////////////////////////////////////////////////////////
class SelectiHistory::Impl {
public:
	using self_t = SelectiHistory;
	self_t* self;

	QStringList lst;

	/////////////////////////////////////////
	Impl( self_t* _self ) :
		self( _self ) {
	}


	/////////////////////////////////////////
	void push( const QString& s ) {
		lst.push_back( s );
		emit self->historyUpdated();
	}

};


//////////////////////////////////////////////////////////////////////////////////
SelectiHistory::SelectiHistory() :
	impl( new Impl( this ) ) {

}


/////////////////////////////////////////
void SelectiHistory::push( const QString& s ) {
	impl->push( s );
}
