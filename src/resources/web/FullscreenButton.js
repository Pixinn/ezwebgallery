/***********************************
*
* FULLSCREEN_BUTTON
*
************************************/
﻿    this.buttonFullscreen = new CButtonToolbar( {
            $handle: p_htmlStructure.toolbar.$buttonFullscreen,
            script: "undefined",

            onClick: function() // Toggles the fullscreen
            {
              if (screenfull.enabled === true ) {
                //A bug in chrome requires the toolbar to be redrawn after entering or exiting fullscreen
                $(".toolbar").hide();
                screenfull.toggle();
                window.setTimeout( function() {  $(".toolbar").show() }, 16 );
              }
            }

    } );

    //this.buttonFullscreen.disable();

    if( screenfull.enabled == false ) {
      this.buttonFullscreen.hide();
    };

    // this.buttonFullscreen.click = function() { // Simulates a click on the button
    //   that.buttonFullscreen.$handle.trigger('click');
    // };
    //
    // this.buttonFullscreen.setFullscreenRequested = function(isFullscreenRequested) {
    //   if(isFullscreenRequested) {
    //     that.buttonFullscreen.$handle.attr("fs_requested","true");
    //   } else {
    //     that.buttonFullscreen.$handle.attr("fs_requested","false");
    //   }
    // };
    //
    // this.buttonFullscreen.isFullscreenRequested = function() {
    //     if(that.buttonFullscreen.$handle.attr("fs_requested") === "true") {
    //         return true;
    //     }
    //     else {
    //         return false;
    //     }
    // };
