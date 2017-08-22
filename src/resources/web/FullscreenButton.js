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
            Mosaic.resize();
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
