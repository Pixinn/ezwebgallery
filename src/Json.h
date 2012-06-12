/* 
 *  EZWebGallery:
 *  Copyright (C) 2012 Christophe Meneboeuf <dev@ezwebgallery.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QString>
#include <QMap>


//////////////////////

//STILL MISSING :
//--------------
//Boolean
//Null
//Array

/////////////////////////

namespace JSON
{     

    class Root;
    class Array;
    class Object;
    class String;
    class Number;
    class Boolean;

    class IValue 
    {
    public:
        virtual ~IValue( void ) {   }
    protected:
        virtual QString serialize( void ) const = 0;
        inline QString& indentate( QString& ) const;
        static const QString TAB;
        static int m_nbIndentation; //To follow indentation    

        friend class Object;
        friend class Array;
    };

     class String : public IValue, public QString
    {
    protected:
        String( const QString & string ) :
            IValue(),
            QString( string ) {   }
        QString serialize( void ) const {
            return QString("\"") + *this + QString("\"");
        }

        friend class Object;
        friend class Array;
    };



    class Number : public IValue
    {
    protected:
        Number( void ) :
            IValue(),
            m_value( 0.0 ) {    }
        Number( double value ) :
            IValue(),
            m_value( value ) {    }
        QString serialize( void ) const {
          return QString::number( m_value );
        }
    private:
        double m_value;

        friend class Object;
        friend class Array;
    };

    
    class Boolean : public IValue
    {
    protected:
        Boolean( bool value ) :
             IValue(),
             m_value( value )
        {   }
        QString serialize( void ) const;
    private:
        bool m_value;

        friend class Object;
        friend class Array;
    };


    class Object : public IValue
    {
    public:
        ~Object( void );
        Object& addObject( const QString& key );
        Array& addArray( const QString& key );
        void addNumber( const QString& key, double number );
        void addBoolean( const QString& key, bool boolean );
        void addString( const QString& key,const QString& str );
     protected:       
        Object( void ) : IValue()  {   } //protected construtor; only called from Root, Object or Array
        QString serialize( void ) const;     
        QMap<QString, IValue*> m_map;

        friend class Array;
    };

   
    class Array : public IValue
    {
    public:
        ~Array( void );
        Object& appendObject( void );
        //Array& appendArray( void );
        void appendNumber( double number );
        void appendBoolean( bool boolean );
        void appendString( const QString& str );
    protected:
        Array( void ) : IValue() {  } //protected construtor; only called from Root, Object or Array
        QString serialize( void ) const;
    private:
        QList<IValue*> m_list;

        friend class Object;
    };
   

    class Root: public Object
    {
    public:
        Root( void ) : Object()
        {   }
        Object& addObject( const QString& key );

        QString serialize( void ) const {
           return Object::serialize();
        }

        void clear( void );
    private:
        QList<IValue*> m_referencedValues;
    };

}
