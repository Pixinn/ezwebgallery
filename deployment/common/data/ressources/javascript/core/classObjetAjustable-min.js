function PhotoAjustable(d,k,l){var b=d;var h=d.parent().parent();var g;var c;var a;var e;var j;var f;var i;g=b.height();c=b.width();e=k;j=l;a=c/g;this.ojectResize=function(){var m=g;var n=0;if(c>i){m=i/a;b.width(i);b.height(m);n=1}if(m>f){b.height(f);b.width(f*a);n=1}if(n==0){b.height(g);b.width(c)}};this.ajuster=function(o,n){var m;m=calculerTaillesMax(o,n,h,e,j);f=m.h;i=m.w;this.ojectResize()}};