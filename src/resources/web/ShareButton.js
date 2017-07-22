
/***********************************
*
* SHARE_BUTTON
*
************************************/
﻿    this.buttonShare = new CButtonToolbar( {
            $handle: p_htmlStructure.toolbar.$buttonShare,
            script: "//static.addtoany.com/menu/page.js",
            onClick: function() { that.screenShare.display();
                                    var nbButtonsByRow = 4;
                                    var $buttons = that.html.share.$buttons;
                                    var buttonSz = $buttons.eq(0).outerWidth() + 2*parseFloat(that.html.share.$buttons.eq(0).css("margin-left"));
                                    that.html.share.$wrapper.width( nbButtonsByRow * buttonSz )
                                                            .height( Math.ceil( $buttons.length / nbButtonsByRow) * buttonSz )
                                                            .verticalCenter(0);
                                }
        }
        );
    this.buttonShare.disable();
