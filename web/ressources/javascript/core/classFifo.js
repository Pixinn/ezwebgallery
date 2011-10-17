/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 The EZWebGallery Dev Team <dev@ezwebgallery.org>
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
 
    
/* Classe Fifo
 * ------------------
 */
 function Fifo( )
{ 
	
	//Private:
	var m_pile = new Array( );
	var m_length = 0;
		
	/* push( pushed )
	* --------------
	* Ajoute un élément dans la fifo
	* In : pushed - élément ajouté
	*/ 
	this.push = function( pushed )
	{
  	 m_pile[m_length] = pushed;
     m_length++;

	}
	
	
	/* insert( inserted )
	* --------------
	* Insert un élément en Nieme position dans la fifo
	* In : inserted - élément inséré
	* In : position de élément inséré (0: 1ere position à poper)
	*/ 
	this.insert = function( inserted, position )
	{
		 //Si position > length, on insère en dernière position
		 if( m_length < position){
				position = m_length;
  	 }
  	 for (var j = position; j < m_length; j++) {
    			m_pile[j + 1] = m_pile[j];
    }
  	 m_pile[position] = inserted;
     m_length++;

	}
	
	
	/* pop( )
	* --------------
	* Retire l'élément le plus ancien de la fifo
	* Return : élément le plus ancien
	*/ 
	this.pop = function ( )
	{
		var retour;
		var i;
		
		 if( m_length > 0)
		 {
			  retour = m_pile[0];
			  for( i = 0; i < m_length-1; i++){
			  	m_pile[i] = m_pile[i+1]
			  }
  		  delete m_pile[m_length-1];
			  m_length--;
		 }
		else{
			retour = "undefined";
		}
		
		return retour;
		
	}
    
  /* read( )
	* --------------
	* Lis l'élément le plus ancien de la fifo
	* Return : élément le plus ancien
	*/  
   this.read = function ( )
	{
		 if( m_length > 0)
		 {
			  return m_pile[0];
		}
		else{
			return "undefined";
		}
		
	}
	
	/* clear( )
	* --------------
	* Efface la pile
	*/  
   this.clear = function ( )
	{
			m_length = 0;		
	}
	
	
}