#include "Json.h"

using namespace JSON;

const QString IValue::TAB("    ");
int IValue::m_nbIndentation = -1;
//Referencer Referencer::m_singleInstance;

/*************************************
* IValue::IValue( void )
*
* Constructor called by all IValue derivatives
*************************************/
IValue::IValue( void )
{
    //Referencer::getInstance( ).addValue( this );
}

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
    QMapIterator<QString,IValue*> it(*this);
    while( it.hasNext() ) {
        IValue* val = it.next().value();
        delete val;
    }
    clear();
}


/*************************************
* ~Array( void )
*
* call all hosted pointers' destructor
*************************************/
Array::~Array( void )
{
    QListIterator<IValue*> it(*this);
    while( it.hasNext() ) {
        IValue* toto =it.next();
        delete toto;
    }
    clear();
}

/*************************************
* Root::insert( const QString& key, Object* object )
*
* Add an object to the root
*************************************/
void Root::insert( const QString& key, Object* object )
{
    m_referencedValues.append( object );
    Object::insert( key, object );
}

/*************************************
* Root::insert( const QString& key, Array* list )
*
* Add an array to the root
*************************************/
void Root::insert( const QString& key, Array* list  )
{
    m_referencedValues.append( list );
    Object::insert( key, list );
}

/*************************************
* Root::insert( const QString& key, String* string )
*
* Add a String to the root
*************************************/
void Root::insert( const QString& key, String* string )
{
    m_referencedValues.append( string );
    Object::insert( key, string );
}

/*************************************
* Root::insert( const QString& key, Number* number )
*
* Add an object to the root
*************************************/
void Root::insert( const QString& key, Number* number )
{
    m_referencedValues.append( number );
    Object::insert( key, number );
}

/*************************************
* Root::insert( const QString& key, Boolean* boolean )
*
* Add a Boolean to the root
*************************************/
void Root::insert( const QString& key, Boolean* boolean )
{
    m_referencedValues.append( boolean );
    Object::insert( key, boolean );
}

/*************************************
* Root::insert( const QString& key, Null* empty )
*
* Add an object to the root
*************************************/
void Root::insert( const QString& key, Null* empty )
{
    m_referencedValues.append( empty );
    Object::insert( key, empty );
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
	QMapIterator<QString,IValue*> iterator(*this);
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
* Boolean::serialize( void )
*
* Serialize the boolean to a QString
*************************************/
QString Boolean::serialize( void ) const
{
  if( m_value == true ) {
      return QString("\"true\"");
  }
  else {
      return QString("\"false\"");
  }
}

/*************************************
* Array::serialize( void )
*
* Serialize the array to a QString
*************************************/
QString Array::serialize( void ) const
{
    //opening bracket
    QString serialized('\n');
    indentate(serialized);
    serialized += "[\n";
    m_nbIndentation++;
    //adding serialized object
	QListIterator<IValue*> iterator(*this);
	while (iterator.hasNext()) {
        indentate( serialized );
		serialized += iterator.next()->serialize() + QString(",\n");
    }
    m_nbIndentation--;
    //removing last ','
    int index = serialized.lastIndexOf( ',' );
    if( index != -1 ) {
      serialized.remove( index, 1 );
    }
    //closing bracket
    indentate(serialized);
    serialized += QString("]");
    
    return serialized;
}


/*************************************
* Referencer::deleteAll( void )
*
* IMPORTANT: destroyes all the object pointed by hosted pointers!!!
*************************************/
/*void Referencer::deleteAll( void )
{
    foreach( IValue* value, m_values ) {
        delete value;
    }
}*/
 
