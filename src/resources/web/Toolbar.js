
// Buttons AND SCREENS!
function Toolbar( p_htmlStructure )
{
    var that = this;
    this.html = p_htmlStructure;
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

    [FULLSCREEN_BUTTON]

    [SHARE_BUTTON]

    this.screenIndex = new CScreen( { $handle: that.html.index.$screen,
                                      deck: that.deck,
                                      listButtonsOther: [ that.buttonShare ],
                                      buttonScreen: that.buttonIndex
                                    }
                                  );
    that.deck.add( that.screenIndex );

    this.screenPhoto = new CScreen( { $handle: that.html.photo.$screen,
                                      deck: that.deck,
                                      listButtonsOther: [ that.buttonIndex, that.buttonShare ],
                                      buttonScreen: "undefined"
                                    }
                                  );
    that.deck.add( that.screenPhoto );

    [SHARE_SCREEN]

    this.buttonIndex.disable();
    that.deck.moveOnTop( that.screenIndex );



    return {
        screenIndex: this.screenIndex,
        screenPhoto: this.screenPhoto,
        showButtons: this.showButtons
    }

}
