
function CObserver( )
{
    this.subscribers = new Array();
    
    CObserver.prototype.subscribe = function( subscriber )
    {
        this.subscribers.push( subscriber );
    };
    
    CObserver.prototype.unsubscribe = function( subscriber )
    {
        for(var i in this.subscribers) {
            if( this.subscribers[i] == subscriber) {
                this.subscribers.splice(i,1);
                break;
             }
        }
    }
    
    CObserver.prototype.fire = function( evt )
    {   //call each subscibed functions
        for( var i = 0; i < this.subscribers.length; i++ ) {
            this.subscribers[i].call( evt );
        }
    }
    
}
