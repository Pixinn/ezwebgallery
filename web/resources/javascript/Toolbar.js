
// Buttons AND SCREENS!
function Toolbar( p_htmlStructure, p_user_interactions  )
{
    var that = this;
    this.html = p_htmlStructure;
    this.ui = p_user_interactions;
    this.deck = new CDeck();
    this.buttons = [];


    // Shows optional buttons
    this.showButtons = function()
    {
        $.each(that.buttons, function( index, button ) {
            button.onLoaded();
        });
    }



    this.buttonIndex = new CButtonToolbar( {    $handle: p_htmlStructure.toolbar.$buttonIndex,
                                                onClick: function() {
                                                    that.screenIndex.display();
                                                },
                                                onLoaded: function() {
                                                    this.$handle.show();
                                                }
                                           } );

     this.buttons.push(this.buttonIndex);
     this.buttonIndex.$handle.hide();

    /***********************************
*
* FULLSCREEN_BUTTON
*
************************************/
this.buttonFullscreen = new CButtonToolbar( {
        $handle: p_htmlStructure.toolbar.$buttonFullscreen,
        script: "undefined",

        onClick: function() // Toggles the fullscreen
        {
          if (screenfull.enabled === true ) {
            //A bug in chrome requires the toolbar to be redrawn after entering or exiting fullscreen
            $(".toolbar").hide();
            screenfull.toggle();
            window.setTimeout( function() {
                $(".toolbar").show();
            }
            , 20 );
            Mosaic.redraw();
          }
      },

      onLoaded: function()
      {
          if( screenfull.enabled ) {
              this.enable();
              this.$handle.show();
          }
      }
    } );

this.buttonFullscreen.$handle.hide();
this.buttons.push(this.buttonFullscreen);
that.buttonFullscreen.disable();


    this.screenIndex = new CScreen( { $handle: that.html.index.$screen,
                                      deck: that.deck,
                                      listButtonsOther: [ that.buttonShare ],
                                      buttonScreen: that.buttonIndex
                                    }
                                  );
    this.screenIndex.eventOnDisplaying.subscribe( function() {
        Mosaic.redraw();
        that.ui.onIndexScreen();
    } );
    that.deck.add( that.screenIndex );

    this.screenPhoto = new CScreen( { $handle: that.html.photo.$screen,
                                      deck: that.deck,
                                      listButtonsOther: [ that.buttonIndex, that.buttonShare ],
                                      buttonScreen: "undefined"
                                    }
                                  );
    that.deck.add( that.screenPhoto );

    this.buttonIndex.disable();
    that.deck.moveOnTop( that.screenIndex );



    return {
        screenIndex: this.screenIndex,
        screenPhoto: this.screenPhoto,
        showButtons: this.showButtons
    }

}
