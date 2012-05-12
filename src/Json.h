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


namespace JSON
{     

    class IValue 
    {
    public:
        IValue( void );
        virtual ~IValue( void ) {   }
    protected:
        virtual QString serialize( void ) const = 0;
        inline QString& indentate( QString& ) const;
        static const QString TAB;
        static int m_nbIndentation; //To follow indentation    

        friend class Object;
        friend class Array;
    };




    class Array : public IValue, public QList<IValue*>
    {
    public:
        Array( void ) :
            IValue(),
            QList<IValue*>() {  }
        Array( const QList<IValue*> & array ) :
            IValue(),
            QList<IValue*>( array )
        {    }
        ~Array( void );
    protected:
        QString serialize( void ) const;
    };



    class Object : public IValue, public QMap<QString, IValue*>
    {
    public:
        Object( void ) :
            IValue(),  QMap<QString, IValue*>()
        {   }
        ~Object( void );
     protected:
        QString serialize( void ) const;
    };

   

    class String : public IValue, public QString
    {
    public:
        String( const QString & string ) :
            IValue(),
            QString( string ) {   }
        String( void ) :
            IValue(),
            QString( "null" ) {   }
        inline void setString( const QString & string )  {
            *this = string;
        }
    protected:
        inline QString serialize( void ) const {
            return QString("\"") + *this + QString("\"");
        }
    };



    class Number : public IValue
    {
    public:
        Number( void ) :
            IValue(),
            m_value( 0.0 ) {    }
        Number( double value ) :
            IValue(),
            m_value( value ) {    }
        inline void setValue( double value ) {
          m_value = value;
        }
        inline double toDouble( void ) const {
          return m_value;
        }
    protected:
        inline QString serialize( void ) const {
          return QString::number( m_value );
        }
    private:
        double m_value;
    };



    class Boolean : public IValue
    {
    public:
        Boolean( void ) :
            IValue(),
            m_value( false )
        {   }        
        Boolean( bool value ) :
            IValue(),
            m_value( value ) {    }
        inline void setValue( bool value ) {
          m_value = value;
        }
        inline bool toBool( void ) const {
          return m_value;
        }
    protected:
        QString serialize( void ) const;
    private:
        bool m_value;
    };




    class Null : public IValue
    {
    public:
        Null( void ) : IValue() {   }
    protected:
        inline QString serialize( void ) const {
          return QString("\"null\"");
        }
    };


    class Root: public Object
    {
    public:
        Root( void ) : Object()
        {   }
        void insert( const QString& key, Object* object );
        void insert( const QString& key, Array* list );
        void insert( const QString& key, String* string );
        void insert( const QString& key, Boolean* boolean );
        void insert( const QString& key, Number* number );
        void insert( const QString& key, Null* empty );
        QString serialize( void ) const {
           return Object::serialize();
        }
    private:
        QList<IValue*> m_referencedValues;
    };

    
    //Singleton referencing all IValue created.
    //Used to free their corresponding allocated ressources
    /*class Referencer
    {
	public:
        static Referencer& getInstance( void ) {
            return m_singleInstance;
        }
        void deleteAll( void );
        inline void addValue( IValue* value ) {
            m_values << value;
        }
        
    private:
        Referencer( void ) {    } //private constructor
        
        static Referencer m_singleInstance;
        QList<IValue*> m_values;
    };*/

}
