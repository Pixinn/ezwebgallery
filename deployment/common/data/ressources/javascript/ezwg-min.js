function CEvent(){this.subscribers=new Array();CEvent.prototype.subscribe=function(a){if(a!=undefined){this.subscribers.push(a)}else{TOOLS.trace("CEvent::Subscribe error")}};CEvent.prototype.unsubscribe=function(b){for(var a in this.subscribers){if(this.subscribers[a]==b){this.subscribers.splice(a,1);break}}};CEvent.prototype.fire=function(a){for(var b=0;b<this.subscribers.length;b++){this.subscribers[b].call(a)}}}function CHashTable(){var b=this;this.len=0;this.items=new Array();for(var a=0;a<arguments.length;a+=2){if(typeof(arguments[a+1])!="undefined"){b.items[arguments[a]]=arguments[a+1];b.len++}}this.removeItem=function(c){if(typeof(b.items[c])!="undefined"){b.len--;delete b.items[c]}};this.addItem=function(c,d){if(typeof(d)!="undefined"&&typeof(b.items[c])=="undefined"){b.len++;b.items[c]=d;return d}else{return"undefined"}};this.getItem=function(c){return b.items[c]};this.hasItem=function(c){return typeof(b.items[c])!="undefined"};this.getLength=function(){return b.len}}function CPhoto(c,b){var a=this;this.nativeSize=b;this.size=TOOLS.clone(b);this.id=c;this.f_loaded=false;this.spinner=new Image();this.spinner.src="ressources/images/ajax-loader.gif";this.image=new Image();CPhoto.prototype.load=function(d,e){if(this.f_loaded==false){$(this.image).load(function(){TOOLS.trace(this.src+" loaded.");a.f_loaded=true;e(a)}).attr("src",d)}};CPhoto.prototype.resize=function(e){var d=TOOLS.clone(this.nativeSize);if(e.w<this.nativeSize.w&&e.h<this.nativeSize.h){d=e}if(this.f_loaded==true){$(this.image).width(d.w).height(d.h)}else{}this.size=d};CPhoto.prototype.verticalCenter=function(d){$(this.getImage()).css("position","relative").verticalCenter(d)};CPhoto.prototype.getImage=function(){if(this.f_loaded==true){return this.image}else{return this.spinner}};CPhoto.prototype.isLoaded=function(){return this.f_loaded}}function CFifo(){var a=this;this.m_stack=new Array();this.m_length=0;this.push=function(b){a.m_stack[a.m_length]=b;a.m_length++};this.pushFirst=function(c){for(var b=a.m_length;b>0;b--){a.m_stack[b]=a.m_stack[b-1]}a.m_stack[0]=c;a.m_length++};this.pop=function(){var b;if(a.m_length>0){b=a.m_stack.shift();a.m_length--}else{b="undefined"}return b};this.size=function(){return a.m_length}}function CStorage(a){var c=this;this.nbItems=a;this.storage=new CHashTable();this.ages=new Array();for(var b=0;b<this.nbItems;b++){this.ages[b]=-1}this.addPhoto=function(d){if(c.storage.hasItem(d.url)==false){c.removeOldest();c.storage.addItem(d.url,new CPhoto(d.id,d.size));c.updateAges(d.url)}TOOLS.trace("Cache free space: "+(c.nbItems-c.storage.getLength()));return c.storage.getItem(d.url)};this.removeOldest=function(){if(c.storage.getLength()>=c.nbItems){var d=c.ages[c.nbItems-1];c.storage.removeItem(d);TOOLS.trace("removing "+d)}};this.updateAges=function(d){for(b=c.nbItems-1;b>0;b--){c.ages[b]=c.ages[b-1]}c.ages[0]=d};this.photoLoaded=function(d){c.storage.getItem(d).isLoaded=true}}function CUserInterractions(b,a){var e=this;this.html=a;this.properties=b;this.windowResizedEvent=new CEvent();this.thumbnailClickedEvent=new CEvent();this.closePhotoEvent=new CEvent();this.previousPhotoEvent=new CEvent();this.previousPhotoEvent=new CEvent();this.nextPhotoEvent=new CEvent();this.previousIndexEvent=new CEvent();this.nextIndexEvent=new CEvent();this.buttonEnabledClass="photoButtonEnabled";this.buttonDisabledClass="photoButtonDisabled";this.previousPhotoId=-1;this.currentPhotoId=1;this.nextPhotoId=2;this.fCurrentlyScrolling=false;this.fEnablePreviousNextRequired=false;var d=37;var f=39;var c=27;this.watchedKeyDown=false;this.start=function(){$(window).resize(function(){e.onWindowResized()}).keydown(function(g){e.onKeyboardIndexSrc(g)});e.enableThumbnailClick();e.html.photo.buttons.$close.click(function(){e.onClosePhoto()});e.disablePreviousNext()};this.disablePreviousNext=function(){e.disablePrevious();e.disableNext()};this.enablePreviousNext=function(){if(!e.fCurrentlyScrolling){e.enablePrevious();e.enableNext();e.fEnablePreviousNextRequired=false}else{e.fEnablePreviousNextRequired=true}};this.enablePrevious=function(){if(e.previousPhotoId>=1){TOOLS.trace("enable previous "+e.previousPhotoId);e.html.photo.buttons.$previous.removeClass().addClass(e.buttonEnabledClass).one("click",e.onPreviousPhoto)}};this.disablePrevious=function(){TOOLS.trace("disable previous");e.html.photo.buttons.$previous.removeClass().addClass(e.buttonDisabledClass).unbind("click")};this.enableNext=function(){if(e.nextPhotoId<=e.properties.photos.list.length){TOOLS.trace("enable next "+e.nextPhotoId);e.html.photo.buttons.$next.removeClass().addClass(e.buttonEnabledClass).one("click",e.onNextPhoto)}};this.disableNext=function(){TOOLS.trace("disable next");e.html.photo.buttons.$next.removeClass().addClass(e.buttonDisabledClass).unbind("click")};this.enableThumbnailClick=function(){e.html.index.mosaic.$thumbnails.click(function(){TOOLS.trace("thumbnail #"+this.id+" clicked.");e.thumbnailClickedEvent.fire(this)})};this.getWindowResizedEvent=function(){return e.windowResizedEvent};this.getThumbnailClickedEvent=function(){return e.thumbnailClickedEvent};this.getClosePhotoEvent=function(){return e.closePhotoEvent};this.getPreviousPhotoEvent=function(){return e.previousPhotoEvent};this.getNextPhotoEvent=function(){return e.nextPhotoEvent};this.getPreviousIndexEvent=function(){return e.previousIndexEvent};this.getNextIndexEvent=function(){return e.nextIndexEvent};this.onWindowResized=function(){e.windowResizedEvent.fire()};this.onClosePhoto=function(){e.closePhotoEvent.fire()};this.onPhotoDisplayedLoaded=function(g){e.previousPhotoId=g-1;e.nextPhotoId=g+1;e.currentPhotoId=g;e.enablePreviousNext()};this.onPreviousPhoto=function(){e.disablePrevious();e.previousPhotoEvent.fire({id:e.previousPhotoId})};this.onNextPhoto=function(){e.disableNext();e.nextPhotoEvent.fire({id:e.nextPhotoId})};this.onScrolling=function(){e.fCurrentlyScrolling=true};this.onScrolled=function(){e.fCurrentlyScrolling=false;e.html.photo.buttons.$close.click(function(){e.onClosePhoto()});e.html.photo.$div.click(function(){e.onClosePhoto()});if(e.fEnablePreviousNextRequired){e.fEnablePreviousNextRequired=false;e.enablePreviousNext()}};this.onIndexScreen=function(){$(window).unbind("keydown").keydown(function(g){e.onKeyboardIndexSrc(g)});e.html.photo.$div.unbind("click");e.enableThumbnailClick()};this.onPhotoScreen=function(){$(window).unbind("keydown").keydown(function(g){e.onKeyboardPhotoScr(g)});e.html.photo.$div.click(function(){e.onClosePhoto()});e.html.index.mosaic.$thumbnails.unbind("click")};this.onKeyboardIndexSrc=function(h){if(e.watchedKeyDown==false){var g=h.keyCode||h.which;switch(g){case d:e.watchedKeyDown=true;e.previousIndexEvent.fire();e.watchedKeyDown=false;break;case f:e.watchedKeyDown=true;e.nextIndexEvent.fire();e.watchedKeyDown=false;break;default:break}}};this.onKeyboardPhotoScr=function(h){if(e.watchedKeyDown==false){var g=h.keyCode||h.which;switch(g){case d:e.watchedKeyDown=true;e.html.photo.buttons.$previous.click();e.watchedKeyDown=false;break;case f:e.watchedKeyDown=true;e.html.photo.buttons.$next.click();e.watchedKeyDown=false;break;case c:e.watchedKeyDown=true;e.onClosePhoto();e.watchedKeyDown=false;break}}}}function CFrameFactory(b){var a=this;this.strPhoto="divPhoto";this.strFrame="cadrePhoto";this.strTitle="photoTitle";this.strClose="boutonIndex";this.strCaption="photoCaption";this.captions=[];for(i=0;i<b.length;i++){this.captions[i]=b[i].caption}this.frame=function(c,d){c.append('<div id="'+a.strFrame+'"><div id="'+a.strTitle+'"><img id="'+a.strClose+'" src="ressources/images/close.gif"></div><div id="'+a.strPhoto+'"></div><div class="'+a.strCaption+'"></div></div>');a.caption(c.find(a.getStrCaption()),d)};this.caption=function(c,d){c.empty().append(a.captions[d]).height(Math.min(c.height(),c.parent().find(a.getStrTitle()).height()))};this.getStrPhoto=function(){return"#"+a.strPhoto};this.getStrFrame=function(){return"#"+a.strFrame};this.getStrTitle=function(){return"#"+a.strTitle};this.getStrClose=function(){return"#"+a.strClose};this.getStrCaption=function(){return"."+a.strCaption}}function CCarrousel(a,b){var c=this;this.properties=a;this.html=b;this.html.photoClass="mainPhoto";this.html.$viewport=$("#photoViewport");this.html.$slides=$(".slide");this.html.$deprecated="NULL";this.html.$previous=$("#previousSlide");this.html.$current=$("#currentSlide");this.html.$next=$("#nextSlide");this.photoDisplayedLoadedEvent=new CEvent();this.scrollingEvent=new CEvent();this.scrolledEvent=new CEvent();this.loader=new CPhotoLoader(a,this.html);this.frameFactory=new CFrameFactory(a.photos.list);this.scrolledAmount=0;this.prevPhoto=-1;this.currPhoto=-1;this.nextPhoto=-1;this.displayPhoto=function(e,g){var d=g.find(c.frameFactory.getStrPhoto());d.find("img").remove();var f=e.getImage();$image=$(f);if(e.isLoaded()==true){$image.removeClass().addClass(c.html.photoClass)}d.append(f);d.width($image.outerWidth()).height($image.outerHeight());d.parent().width(d.outerWidth()).height(d.outerHeight()+d.siblings().outerHeight()).verticalCenter(0);TOOLS.trace("parent: "+$(c.frameFactory.getStrFrame()).width()+" - "+$(c.frameFactory.getStrFrame()).parent().height());TOOLS.trace("div: "+d.width()+" - "+d.height());TOOLS.trace("img: "+$image.width()+" - "+$image.height())};this.centerViewport=function(){c.html.$viewport.scrollLeft(c.html.$current.position().left)};this.previous=function(d){c.setCurrentPhoto(d);var e=c.loader.load(c.currPhoto,c.loader.PREVIOUS);c.currentPhoto=e;c.frameFactory.frame(c.html.$previous,d-1);c.displayPhoto(e,c.html.$previous);c.scroll(c.html.$previous);return e};this.load=function(e){c.setCurrentPhoto(e);var d=c.loader.load(e,c.loader.NEXT);c.displayPhoto(d,c.html.$current);c.frameFactory.caption(c.html.$caption,e-1);c.html.$viewport.scrollLeft(c.html.$current.position().left);c.currentPhoto=d;return d};this.next=function(d){c.setCurrentPhoto(d);var e=c.loader.load(c.currPhoto,c.loader.NEXT);c.currentPhoto=e;c.frameFactory.frame(c.html.$next,d-1);c.displayPhoto(e,c.html.$next);c.scroll(c.html.$next);return e};this.getCurrentPhoto=function(){return c.currentPhoto};this.setSpace=function(d){c.loader.space=d};this.updateHandles=function(){c.html.$deprecated=$(".deprecated");c.html.$previous=$("#previousSlide");c.html.$current=$("#currentSlide");c.html.$next=$("#nextSlide");c.html.$frame=c.html.$current.find(c.frameFactory.getStrFrame());c.html.$div=c.html.$frame.find(c.frameFactory.getStrPhoto());c.html.$title=c.html.$frame.find(c.frameFactory.getStrTitle());c.html.$caption=c.html.$frame.find(c.frameFactory.getStrCaption());c.html.buttons.$close=c.html.$title.find(c.frameFactory.getStrClose());c.html.$slides=$(".slide")};this.setCurrentPhoto=function(d){c.currPhoto=d;c.prevPhoto=d-1;c.nextPhoto=d+1};this.scroll=function(d){c.scrollingEvent.fire();c.scrolledAmount=d.position().left;c.html.$frame.addClass("deprecated");if(d==c.html.$previous){$("#slidesContainer").animate({left:"+="+c.html.$current.outerWidth()+"px"},"fast","swing",function(){c.onScrolled("BACKWARD")});c.html.$next.addClass("deprecated");c.html.$current.attr("id","nextSlide");c.html.$previous.attr("id","currentSlide")}else{$("#slidesContainer").animate({left:"-="+c.html.$current.outerWidth()+"px"},"fast","swing",function(){c.onScrolled("FORWARD")});c.html.$previous.addClass("deprecated");c.html.$current.attr("id","previousSlide");c.html.$next.attr("id","currentSlide")}c.updateHandles()};this.onPhotoLoaded=function(){if(this.id==c.currPhoto){c.displayPhoto(this,c.html.$current);c.photoDisplayedLoadedEvent.fire(this)}};this.loader.getPhotoLoadedEvent().subscribe(c.onPhotoLoaded);this.onScrolled=function(d){c.html.$deprecated.remove();if(d=="FORWARD"){$("#slidesContainer").css("left","0px");c.html.$current.parent().append('<div id="nextSlide" class="slide"></div>')}else{c.html.$current.parent().prepend('<div id="previousSlide" class="slide"></div>');$("#slidesContainer").css("left","0px")}c.updateHandles();c.scrolledEvent.fire()};this.getPhotoDisplayedLoadedEvent=function(){return c.photoDisplayedLoadedEvent};this.getScrollingEvent=function(){return c.scrollingEvent};this.getScrolledEvent=function(){return c.scrolledEvent}}function CPhotoLoader(a,b){var c=this;this.NEXT=1;this.PREVIOUS=-1;this.properties=a;this.html=b;this.$spinner=$(new Image());this.$spinner.attr("src",this.html.spinnerSrc);this.storage=new CStorage(a.photos.technical.cacheSize);this.prefetchingQueue=new CFifo();this.space={h:0,w:0};this.photoLoadedEvent=new CEvent();this.fetch=function(f){TOOLS.trace("Fetching "+f);var e=c.computeUrl(f);var d=c.storage.addPhoto(e);if(d.isLoaded()==false){d.load(e.url,function(g){c.photoLoadedEvent.fire(g);if(c.prefetchingQueue.size()>0){c.fetch(c.prefetchingQueue.pop())}})}else{if(c.prefetchingQueue.size()>0){c.fetch(c.prefetchingQueue.pop())}}return d};this.load=function(g,f){for(var d=c.properties.photos.technical.prefetchSize;d>0;d--){var e=g+(f*d);if(e<=c.properties.photos.list.length&&e>0){c.prefetchingQueue.pushFirst(e)}}return c.fetch(g)};this.getPhotoLoadedEvent=function(){return c.photoLoadedEvent};this.computeUrl=function(e){var d;if(this.properties.photos.technical.qualityStrategy===0){d=this.getSmallerImage(e)}else{d=this.getBiggerImage(e)}TOOLS.trace("space: "+c.space.w+" "+c.space.h+" / "+d.url);d.id=e;return d};this.getBiggerImage=function(g){var d=false;var h=this.properties.photos.list[g-1].sizes;var j=new Object();$.each(h,function(l,k){if(k.width>=c.space.w||k.height>=c.space.h){j[l]=k;d=true}});var f=Number.MAX_VALUE;var e=this.properties.photos.technical.largestSet;$.each(j,function(l,k){if(k.width<f){f=j[l].width;e=l}});return{url:this.properties.defines.IMAGES_PATH+"/"+e+"/"+this.properties.photos.list[g-1].filename,size:{w:h[e].width,h:h[e].height}}};this.getSmallerImage=function(f){var d=false;var h=this.properties.photos.list[f-1].sizes;var j=new Object();$.each(h,function(l,k){if(k.width<=c.space.w&&k.height<=c.space.h){j[l]=k;d=true}});var g=0;var e=this.properties.photos.technical.smallestSet;$.each(j,function(l,k){if(k.width>g){g=j[l].width;e=l}});return{url:this.properties.defines.IMAGES_PATH+"/"+e+"/"+this.properties.photos.list[f-1].filename,size:{w:h[e].width,h:h[e].height}}}}function CUrl(c){var b=this;this.photoStr="Photo";this.photoList=c;this.setHash=function(d){window.location.hash=b.photoStr+d};this.clearHash=function(){window.location.hash=""};this.display=function(f){var e=window.location.hash;var d=parseInt(e.substring(e.indexOf(b.photoStr)+b.photoStr.length));if(a(d)&&d<=b.photoList.length){f(d)}else{b.clearHash()}};function a(d){return !isNaN(parseInt(d))&&isFinite(d)}}function CDisplay(a,b){var c=this;this.html=b;this.html.photo.spinnerSrc="ressources/images/ajax-loader.gif";this.properties=a;this.carrousel=new CCarrousel(a,this.html.photo);this.url=new CUrl(a.photos.list);this.idCurrentPhoto=-1;this.photoScreenEvent=new CEvent();this.indexScreenEvent=new CEvent();this.disableUISignal=new CEvent();this.enableUISignal=new CEvent();this.displayedPhotoLoadedEvent=new CEvent();this.availableSpace={h:0,w:0};$(window).resize(function(){c.setSpace(c.computeAvailableSpace());if(c.html.photo.$screen.is(":visible")===true){c.fitPhoto(c.carrousel.getCurrentPhoto());c.html.photo.buttons.$previous.verticalCenter(0);c.html.photo.buttons.$next.verticalCenter(0);c.carrousel.centerViewport()}});this.displayCurrentUrl=function(){c.url.display(c.displayPhoto)};this.displayPhoto=function(d){c.idCurrentPhoto=d;c.url.setHash(d);c.html.photo.$screen.fadeIn();c.html.photo.buttons.$next.verticalCenter(0);c.html.photo.buttons.$previous.css("top",c.html.photo.buttons.$next.css("top"));c.setSpace(c.computeAvailableSpace());c.load(d,c.carrousel.load);c.photoScreenEvent.fire({id:d})};this.hidePhoto=function(){c.url.clearHash();c.html.photo.$screen.fadeOut("fast");c.indexScreenEvent.fire()};this.getPhotoDisplayedLoadedEvent=function(){return c.displayedPhotoLoadedEvent};this.getPhotoScreenEvent=function(){return c.photoScreenEvent};this.getIndexScreenEvent=function(){return c.indexScreenEvent};this.getDisableUISignal=function(){return c.disableUISignal};this.getEnableUISignal=function(){return c.enableUISignal};this.getScrollingEvent=function(){return c.carrousel.getScrollingEvent()};this.getScrolledEvent=function(){return c.carrousel.getScrolledEvent()};this.onPrevious=function(){c.url.setHash(--c.idCurrentPhoto);c.load(c.idCurrentPhoto,c.carrousel.previous)};this.onNext=function(){c.url.setHash(++c.idCurrentPhoto);c.load(c.idCurrentPhoto,c.carrousel.next)};this.onPhotoDisplayedLoaded=function(){c.displayedPhotoLoadedEvent.fire(this);c.fitPhoto(this)};c.carrousel.getPhotoDisplayedLoadedEvent().subscribe(this.onPhotoDisplayedLoaded);this.setSpace=function(d){c.availableSpace=d;c.carrousel.setSpace(d)};this.load=function(f,e){c.disableUISignal.fire();var d=e(f);if(d.isLoaded()==true){c.displayedPhotoLoadedEvent.fire(d)}c.fitPhoto(d)};this.computeAvailableSpace=function(){var l=6;var f=33;var k=33;var e=0;for(var g=0;g<c.html.photo.$wrapper.siblings().length;g++){e+=c.html.photo.$wrapper.siblings().eq(g).outerWidth()}var j=parseInt(c.html.photo.$frame.css("padding-top").replace("px",""));var d=c.html.photo.$wrapper.innerHeight()-2*j-2*c.properties.photos.technical.decoration.padding-f;var h=$(window).width()-e-2*j-2*c.properties.photos.technical.decoration.padding-l;if(d>c.properties.photos.technical.maxSize.height){d=c.properties.photos.technical.maxSize.height}if(h>c.properties.photos.technical.maxSize.width){h=c.properties.photos.technical.maxSize.width}return{h:d,w:h}};this.fitPhoto=function(d){var f=d.nativeSize.h/d.nativeSize.w;var e={};if(f>1){e.h=c.availableSpace.h;e.w=Math.round(e.h/f);if(e.w>c.availableSpace.w){e.w=c.availableSpace.w;e.h=Math.round(e.w*f)}}else{e.w=c.availableSpace.w;e.h=Math.round(e.w*f);if(e.h>c.availableSpace.h){e.h=c.availableSpace.h;e.w=Math.round(e.h/f)}}d.resize(e);c.html.photo.$div.width(d.size.w+2*c.properties.photos.technical.decoration.padding).height(d.size.h+2*c.properties.photos.technical.decoration.padding);c.html.photo.$frame.width(c.html.photo.$div.outerWidth()).height(c.html.photo.$div.outerHeight()+c.html.photo.$title.outerHeight()+c.html.photo.$caption.outerHeight()).css("position","relative").verticalCenter(0);d.verticalCenter(0)}}CScroller=function(b){this.options=b;var a=this;this.options.$navigation.css("cursor","pointer").click(function(){var d=$(this).attr("target");var c=a.options.$viewport.find(d);a.scrollTo($(this),c)});CScroller.prototype.scrollTo=function(d,c){a.options.$viewport.animate({scrollLeft:c.position().left},a.options.duration,a.options.easing,function(){a.options.onAfter(d,c)})};CScroller.prototype.scrollToPageNr=function(e,g){var d=a.options.$navigation.eq(e-1);var f=a.options.$navigation.eq(g-1).attr("target");var c=a.options.$viewport.find(f);a.scrollTo(d,c)}};CProgressBar=function(a){var b=this;this.options=a;this.nbThumbnailsLoaded=0;CProgressBar.prototype.onLoad=function(){b.nbThumbnailsLoaded++;b.options.$progessBar.reportprogress(b.nbThumbnailsLoaded,b.options.nbThumbnails)};CProgressBar.prototype.show=function(){b.options.$progessBar.parent().show().verticalCenter(0)};CProgressBar.prototype.hide=function(){b.options.$progessBar.parent().hide()}};function CMosaic(a,b){this.buildHtml=function(){var l="slidingPanel";var d="thumbBox";for(var j=1;j<=c.mosaicNbPanels;j++){c.htmlStructure.index.mosaic.$scrollContainer.append('<div class="'+l+'" id="index'+j+'"></div>')}var e=$("."+l);for(var j=1;j<=c.mosaicNbPanels;j++){c.htmlStructure.index.mosaic.$tabsContainer.append('<li class="tab" id="'+j+'" target="#index'+j+'">'+j+"</li>")}c.htmlStructure.index.mosaic.$tabs=c.htmlStructure.index.mosaic.$tabsContainer.find("li");var k=1;for(var h=0;h<e.length;h++){var g=1;while((g<=c.mosaicNbThumbsByPanels)&&(k<=c.mosaicNbThumbnails)){e.eq(h).append('<div class="'+d+'" id="'+k+'"></div>');k++;g++}}var f=$("."+d);c.htmlStructure.index.mosaic.$thumbBoxes=f;this.thumbnailsSet=this.computeThumbnailsSet();c.ThumbnailSize=c.properties.index.mosaic.sizes[this.thumbnailsSet];f.each(function(n){var m=c.properties.photos.list[n].filename;var o=new Image();$(this).append(o);$(o).load(function(){c.oneThumbLoadedEvent.fire("thumbnailLoaded")}).attr("id",this.id).attr("src",src=c.properties.defines.URL_THUMBS_PATH+"/"+c.thumbnailsSet+"/"+m)});c.htmlStructure.index.mosaic.$pannels=e;return c.htmlStructure};this.onPreviousIndex=function(){if(c.currentPageNr>1){c.currentPageNr--;c.scroller.scrollToPageNr(c.currentPageNr,c.currentPageNr)}};this.onNextIndex=function(){if(c.currentPageNr<c.mosaicNbPanels){c.currentPageNr++;c.scroller.scrollToPageNr(c.currentPageNr,c.currentPageNr)}};this.onThumbnailLoaded=function(){if(++c.nbThumbnailLoaded==c.mosaicNbThumbnails){c.allThumbsLoadedEvent.fire()}};this.onThumbnailsLoaded=function(){var l=c.htmlStructure.index.mosaic.$scrollViewport;var h=c.htmlStructure.index.mosaic.$thumbBoxes;var j=h.find("img");var m=j.first().css("border-top-width");m=m.replace("px","");h.width(c.ThumbnailSize+2*m).height(c.ThumbnailSize+2*m);var g=c.properties.index.mosaic.nbCols*h.outerWidth();var f=c.properties.index.mosaic.nbRows*h.outerHeight();l.width(g).height(f);c.htmlStructure.index.mosaic.$pannels.width(g).height(f);c.htmlStructure.index.mosaic.$container.width(l.outerWidth());c.htmlStructure.index.mosaic.$thumbnails=j;c.show();var d=c.htmlStructure.index.mosaic.$pannels;var k=c.htmlStructure.index.mosaic.$scrollContainer;d.css({"float":"left",position:"relative"});k.css("width",d[0].offsetWidth*d.length);l.scrollLeft(0);var e={$viewport:l,$navigation:c.htmlStructure.index.mosaic.$tabs,duration:500,easing:"swing",onAfter:c.highlightNav};c.scroller=new CScroller(e);c.hide();c.highlightNav(c.htmlStructure.index.mosaic.$tabs.eq(0))};this.show=function(){c.htmlStructure.index.mosaic.$container.show().verticalCenter(0);c.htmlStructure.index.mosaic.$thumbBoxes.find("img").verticalCenter(0)};this.hide=function(){c.htmlStructure.index.mosaic.$container.hide()};this.onResize=function(){c.htmlStructure.index.mosaic.$container.verticalCenter(0)};this.onPreviousNext=function(d){var e=Math.ceil(d.id/(c.properties.index.mosaic.nbRows*c.properties.index.mosaic.nbCols));if(e!=c.currentPageNr){TOOLS.trace("Mosaic - new page: "+e);c.scroller.scrollToPageNr(e,e)}};this.getLoadingEvent=function(){return c.oneThumbLoadedEvent};this.getLoadedEvent=function(){return c.allThumbsLoadedEvent};this.computeThumbnailsSet=function(){var f=false;var e=c.properties.index.mosaic.defaultSet;var d=c.htmlStructure.index.$screen.width()-c.properties.index.mosaic.unavailable.horizontal-c.mosaicSizeMargin/2;var g=c.htmlStructure.index.$screen.height()-c.properties.index.mosaic.unavailable.vertical-c.mosaicSizeMargin;$.each(c.properties.index.mosaic.sizes,function(j,h){if(!f&&(d>h*c.properties.index.mosaic.nbCols)&&(g>h*c.properties.index.mosaic.nbRows)){e=j;f=true}});return e};this.highlightNav=function(d){$(".navTabSelected").removeClass("navTabSelected");d.addClass("navTabSelected");c.currentPageNr=parseInt(d.attr("id"))};var c=this;this.properties=a;this.htmlStructure=b;this.scroller;this.mosaicSizeMargin=100;this.mosaicNbThumbnails=c.properties.photos.list.length;this.mosaicNbThumbsByPanels=(c.properties.index.mosaic.nbRows*c.properties.index.mosaic.nbCols);this.mosaicNbPanels=Math.ceil(c.mosaicNbThumbnails/c.mosaicNbThumbsByPanels);this.nbThumbnailLoaded=0;this.ThumbnailSize=0;this.currentPageNr=1;this.oneThumbLoadedEvent=new CEvent();this.oneThumbLoadedEvent.subscribe(this.onThumbnailLoaded);this.allThumbsLoadedEvent=new CEvent();this.allThumbsLoadedEvent.subscribe(this.onThumbnailsLoaded)}Defines={URL_THUMBS_PATH:"thumbnails",IMAGES_PATH:"images",FPS:25};TOOLS={trace:function(b){if(window.console){var a=new Date();console.log(a.getTime()+": "+b)}},clone:function(b){var a={};jQuery.extend(a,b);return a}};jQuery.fn.verticalCenter=function(a){return this.each(function(){var d=jQuery(this);var b=d.parent();if(d.is(":visible")&&b.is(":visible")){var c;c=(b.height()-d.outerHeight())/2-a;c=Math.max(c,0);d.css("top",c)}else{TOOLS.trace("Vertical center: object or its parent not visible!")}})};$(document).ajaxError(function(){TOOLS.trace("Error ajax: "+$(this).attr("src"))});$(document).ready(function(){g_properties.defines=Defines;jQuery.fx.interval=1000/Defines.FPS;HtmlStructure={progressBar:{$box:$("#progressbarWrapper"),$bar:$("#progressbar"),},index:{$screen:$("#screenIndex"),mosaic:{$container:$("#indexSliderContainer"),$scrollViewport:$(".thumbsWrapper"),$scrollContainer:$(".scrollContainer"),$tabsContainer:$("ul.indexNavigation")}},photo:{$screen:$("#screenPhoto"),$frame:$("#cadrePhoto"),$wrapper:$("#wrapperAffichagePhoto"),$div:$("#divPhoto"),$title:$("#photoTitle"),$caption:$(".photoCaption"),buttons:{$previous:$("#boutonPrevious"),$next:$("#boutonNext"),$close:$("#boutonIndex")}}};Mosaic=new CMosaic(g_properties,HtmlStructure);progressBar=new CProgressBar({$progessBar:HtmlStructure.progressBar.$bar,nbThumbnails:g_properties.photos.list.length});Mosaic.getLoadingEvent().subscribe(progressBar.onLoad);Mosaic.getLoadedEvent().subscribe(function(){progressBar.hide();Mosaic.show();Mosaic.getLoadingEvent().unsubscribe(progressBar.onLoad());HtmlStructure.progressBar.$box.remove();delete HtmlStructure.progressBar.$box});var b=new CDisplay(g_properties,HtmlStructure);var a;Mosaic.getLoadedEvent().subscribe(function(){a=new CUserInterractions(g_properties,HtmlStructure);a.start();b.getPhotoDisplayedLoadedEvent().subscribe(function(){a.onPhotoDisplayedLoaded(this.id)});b.getPhotoScreenEvent().subscribe(a.onPhotoScreen);b.getIndexScreenEvent().subscribe(a.onIndexScreen);b.getDisableUISignal().subscribe(a.disablePreviousNext);b.getEnableUISignal().subscribe(a.enablePreviousNext);b.getScrollingEvent().subscribe(a.onScrolling);b.getScrolledEvent().subscribe(a.onScrolled);a.getWindowResizedEvent().subscribe(Mosaic.onResize);a.getThumbnailClickedEvent().subscribe(function(){b.displayPhoto(parseInt(this.id))});a.getClosePhotoEvent().subscribe(b.hidePhoto);a.getPreviousPhotoEvent().subscribe(b.onPrevious);a.getPreviousPhotoEvent().subscribe(function(){Mosaic.onPreviousNext(this)});a.getNextPhotoEvent().subscribe(b.onNext);a.getNextPhotoEvent().subscribe(function(){Mosaic.onPreviousNext(this)});a.getPreviousIndexEvent().subscribe(Mosaic.onPreviousIndex);a.getNextIndexEvent().subscribe(Mosaic.onNextIndex);b.displayCurrentUrl()});progressBar.show();HtmlStructure=Mosaic.buildHtml()});