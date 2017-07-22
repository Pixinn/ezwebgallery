
// Buttons AND SCREENS!
function Toolbar( p_htmlStructure )
{
    var that = this;
    this.html = p_htmlStructure;
    this.deck = new CDeck();

    that.isFullscreenButton = p_htmlStructure.toolbar.$buttonFullscreen.length != 0; ///< Is the optional fullscreen button present?

    this.buttonIndex = new CButtonToolbar( {    $handle: p_htmlStructure.toolbar.$buttonIndex,
                                                onClick: function()
                                                {
                                                //   if (that.isFullscreenButton && screenfull.enabled)  //using screenfull.js
                                                //   {
                                                //     if( screenfull.isFullscreen ) {
                                                //       that.buttonFullscreen.click();
                                                //       p_htmlStructure.toolbar.$buttonFullscreen.attr("fs_requested","true");
                                                //     }
                                                //     else {
                                                //       p_htmlStructure.toolbar.$buttonFullscreen.attr("fs_requested","false");
                                                //     }
                                                //   }
                                                  that.screenIndex.display();
                                                  //that.buttonFullscreen.disable();
                                                }
                                           }
                                         );

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
        screenPhoto: this.screenPhoto
    }

}
