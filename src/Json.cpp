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
* Boolean::serialize( void )
*
* serialize
*************************************/
QString Boolean::serialize( void ) const
{
    if( m_value ) {
        return QString("true");
    }
    else {
        return QString("false");
    }
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
* adds an object to the object
* return: a handle to the added object
*************************************/
Object& Object::addObject( const QString& key)
{
    Object* obj = new Object();
    m_map.insert( key, obj );
    return *obj;
}

/*************************************
* addArray( const QString& key)
*
* adds an array to the object
* return: a handle to the added array
**************************************/
Array& Object::addArray( const QString& key )
{
    Array* arr = new Array();
    m_map.insert( key, arr );
    return *arr;
}

/*************************************
* addNumber( const QString& key, double number )
*
* adds a number to the object
*************************************/
void Object::addNumber( const QString& key, double number )
{
    m_map.insert( key, new Number( number ) );
}

/*************************************
* addBoolean( const QString& key,, bool boolean )
*
* adds a bbolean to the object
*************************************/
void Object::addBoolean( const QString& key, bool boolean )
{
    m_map.insert( key, new Boolean( boolean ) );
}

/*************************************
* addString( const QString& key,const QString& str )
*
* adds a string to the object
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
* Array::~Array( void )
*
* Destructor: deletes all hosted values
*************************************/
Array::~Array( void )
{
    foreach( IValue* value, m_list ) {
        delete value;
    }
    m_list.clear();
}


/*************************************
* appendObject( void )
*
* appends an object to the array
* return: a handle to the added object
*************************************/
Object& Array::appendObject( void )
{
    Object* obj = new Object();
    m_list.append( obj );
    return *obj;
}


/*************************************
* appendArray( void )
*
* appends an array to the array
* return: a handle to the added array
*************************************/
/*Array& Array::appendArray( void )
{
    Array* arr = new Array();
    m_list.append( arr );
    return *arr;
}*/


/*************************************
* addNumber( const QString& key, double number )
*
* appends a number to the array
*************************************/
void Array::appendNumber( double number )
{
    m_list.append( new Number( number ) );
}

/*************************************
* addBoolean( const QString& key,, bool boolean )
*
* appends a bbolean to the array
*************************************/
void Array::appendBoolean( bool boolean )
{
    m_list.append( new Boolean( boolean ) );
}

/*************************************
* addString( const QString& key,const QString& str )
*
* appends a string to the array
*************************************/
void Array::appendString( const QString& str )
{
    m_list.append( new String( str ) );
}


/*************************************
* String serialize( void )
*
* serializes the array
*************************************/
QString Array::serialize( void ) const
{
    //opening brace
    QString serialized('\n');
    indentate( serialized );
    serialized += "[\n";
    m_nbIndentation++;
    //adding serialized object
    foreach( IValue* value, m_list )  {
        indentate( serialized );
        serialized += value->serialize() + QString(",\n");
    }
    m_nbIndentation--;
    //removing last ','
    int index = serialized.lastIndexOf( ',' );
    if( index != -1 ) {
      serialized.remove( index, 1 );
    }
    //closing brace
    indentate(serialized);
    serialized += QString("]");    
    
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
 

/*************************************
* Root::clear( )
*
* Empty the root object
*************************************/
void Root::clear( void )
{
    foreach( IValue* val, m_map ) {
        delete val;
    }
    m_map.clear();
}
 

/*************************************
*  Root::serialize( const QString& varName )
*
* In: (QString) NOT TRUE JSON BECAUSE OF THAT
* RATIONAL: SOME BROWSER CANNOT IMPORT JSON OFFLINE
*************************************/
QString Root::serialize( const QString& varName ) const
{
    return varName + " = " + Object::serialize();
}
