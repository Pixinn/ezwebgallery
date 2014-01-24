
//options:
//-----------
// $progessBar: the progress bar
// nbThumbnails: Number of thumbnails to be loaded

CProgressBar = function( options )
{
    // +++ Attributes +++
    var that = this;
    this.options = options;
    this.nbThumbnailsLoaded = 0;
    
    //+++ Methods +++
    CProgressBar.prototype.onLoad = function()
    {
        that.nbThumbnailsLoaded++;
        that.options.$progessBar.reportprogress( that.nbThumbnailsLoaded, that.options.nbThumbnails );   
    }
    
    CProgressBar.prototype.show = function()
    {
         that.options.$screen.show();
         that.options.$progessBar.parent().verticalCenter( 0 );
    }
    
    CProgressBar.prototype.hide = function()
    {
         that.options.$screen.hide();
    }
    
}
