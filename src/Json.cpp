#include "Json.h"

using namespace JSON;

const QString IValue::TAB("    ");
int IValue::m_nbIndentation = -1;
//Referencer Referencer::m_singleInstance;


/*************************************
* IValue::indentate( QString &string )
*
* insert spaces corresponding to indentation
*************************************/
QString& IValue::indentate( QString &string ) const
{
    for( int i = 0; i < m_nbIndentation; i++ ) {
        string += TAB;
    }
    return string;
}


/*************************************
* ~Object( void )
*
* call all hosted pointers' destructor
*************************************/
Object::~Object( void )
{
    foreach( IValue* val, m_map ) {
        delete val;
    }
    m_map.clear();
}


/*************************************
* addObject( const QString& key)
*
* add an object to the object
* return: a handle to the added object
*************************************/
Object& Object::addObject( const QString& key)
{
    Object* obj = new Object();
    m_map.insert( key, obj );
    return *obj;
}

/*************************************
* addNumber( const QString& key, double number )
*
* add a number to the object
*************************************/
void Object::addNumber( const QString& key, double number )
{
    m_map.insert( key, new Number( number ) );
}


/*************************************
* addString( const QString& key,const QString& str )
*
* add a string to the object
*************************************/
void Object::addString( const QString& key,const QString& str )
{
    m_map.insert( key, new String( str ) );
}



/*************************************
* Object::serialize( void )
*
* Serialize the object to a QString
*************************************/
QString Object::serialize( void ) const
{    
    //opening brace
    QString serialized('\n');
    indentate( serialized );
    serialized += "{\n";
    m_nbIndentation++;
    //adding serialized object
	QMapIterator<QString,IValue*> iterator(m_map);
	while( iterator.hasNext() ) {
		iterator.next();
        indentate( serialized );
	    serialized += QString('\"') + iterator.key() + QString('\"') + QString(':') + iterator.value()->serialize() + QString(",\n");
	}
    m_nbIndentation--;
    //removing last ','
    int index = serialized.lastIndexOf( ',' );
    if( index != -1 ) {
      serialized.remove( index, 1 );
    }
    //closing brace
    indentate(serialized);
    serialized += QString("}");    
    
    return serialized;
}



/*************************************
* Root::insert( const QString& key )
*
* Add an object to the root
*************************************/
Object& Root::addObject( const QString& key )
{
    return Object::addObject( key );
}
 
