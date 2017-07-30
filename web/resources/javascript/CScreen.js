/*
 *  EZWebGallery:
 *  Copyright (C) 2014 Christophe Meneboeuf <xtof@ezwebgallery.org>
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

function CDeck()
{
    var that = this;
    this.storage = new Array();

    this.add = function( screen )   {
        that.storage.push( screen );
    }

    this.getTop = function( )
    {
        return that.storage[ that.storage.length - 1 ];
    }

    this.moveOnTop = function( screen )
    {
        var idx = that.storage.indexOf( screen );
        if( idx !== -1 ) {
            var idxTop = that.storage.length - 1;
            oldScrTop = that.storage[ idxTop ];
            that.storage[ idxTop ] = that.storage[ idx ];
            that.storage[ idx ] = oldScrTop;
        }
    }

    this.removeFromTop = function()
    {
        //swapping the topmost with the one beaneath it
        if( that.storage > 1 ) {
            var idxTop = that.storage.length - 1;
            oldScrTop = that.storage[ idxTop ];
            that.storage[ idxTop ] = that.storage[ idxTop - 1 ];
            that.storage[ idxTop - 1 ] = oldScrTop;
        }

    }
}

function CScreen( p_options )
{
    var that = this;
    that.$handle = p_options.$handle;
    that.deck = p_options.deck;
    that.listButtonsOther = p_options.listButtonsOther;
    that.buttonScreen = p_options.buttonScreen;

    this.eventOnDisplaying = new CEvent();
    this.eventOnDisplayed = new CEvent();
    this.eventOnHiding = new CEvent();
    this.eventOnHidden = new CEvent();


    this.display = function()
    {
        if( that.buttonScreen !== "undefined" ) {
            that.buttonScreen.disable();
        }
        that.$handle.css("z-index","100")
                    .fadeIn( "fast", function()
                    {
                        for ( var i = 0; i < that.listButtonsOther.length; i++)
                        {
                            if( typeof that.listButtonsOther[ i ] !== "undefined" ) {
                                that.listButtonsOther[ i ].enable();
                            }
                        }
                        scr = that.deck.getTop();
                        scr.hide();
                        that.deck.moveOnTop( that );
                        that.eventOnDisplayed.fire();
                    } );
        that.eventOnDisplaying.fire();
    }


    this.hide = function()
    {
        that.eventOnHiding.fire();
        that.$handle.css("z-index","-1")
                    .hide();
        that.eventOnHidden.fire();
    }
};


function CButtonToolbar( p_options )
{
    var that = this;
    that.$handle = p_options.$handle;
    that.onClick = p_options.onClick;   // When clicking on the button
    that.onLoaded = p_options.onLoaded;  // When the Index is fully loaded
    if( typeof p_options.script !== "undefined" && p_options.script.indexOf("//") != -1 ) {
        that.script = p_options.script;
        that.isLocked = true; //locked until the script is loaded
     }
    else   {
        that.script = "undefined";
        that.isLocked = false;
    }

    this.enable = function()
    {
        if( !that.isLocked ) {
        that.$handle.css( "opacity", "1")
                    .css( "cursor", "pointer" )
                    .unbind()
                    .click( that.onClick );
    }
    }

    this.disable = function()
    {
        that.$handle.css( "opacity", "0.4")
                    .css( "cursor", "not-allowed" )
                    .unbind();
    }


    if( that.script != "undefined" ) {
        $.getScript( that.script,
                function() {
                                that.isLocked = false;
                                that.enable();
                            } )
    }
    that.$handle.show(); //displaying optional buttons (hidden by default)
    this.enable();
};
