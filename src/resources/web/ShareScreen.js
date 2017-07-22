
/***********************************
*
* SHARE_SCREEN
*
************************************/

﻿    this.screenShare = new CScreen( {   $handle: that.html.share.$screen,
                                        deck: that.deck,
                                        listButtonsOther: [ that.buttonIndex ],
                                        buttonScreen: that.buttonShare
                                    } );

    that.deck.add( that.screenShare );
