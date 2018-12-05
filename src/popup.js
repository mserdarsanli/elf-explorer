// This needs to be applied manually for testing, set an id to 'qwe' on an element,
// paste this in console, and click that element

function addPopup( ev ) {
  ev.stopPropagation();
  console.log( 'addpopup called with event: ', ev );

  ev.target.style.position = 'relative';

  d1 = document.createElement( 'div' );
  d1.addEventListener( 'click', function( ev ) {
    // Is there a better way to do this?
    ev.stopPropagation();
  } );
  d1.innerHTML = '<b>SHT_RELA:</b> Relocation entries with addends';
  d1.style.position = 'absolute';
  d1.style.width = '300px';
  d1.style.top = '0%';
  d1.style.left = '50%';
  d1.style.transform = 'translate(-50%, calc( -100% - 10px ) )';
  d1.style.backgroundColor = '#eeeeee';
  d1.style.border = '1px solid black';
  d1.style.padding = '5px';

  a = document.createElement( 'a' );
  a.innerText = '[X]';
  a.addEventListener( 'click', function( ev ) {
    ev.stopPropagation();
    ev.target.parentNode.remove();
  } );
  d1.appendChild( a );

  tip = document.createElement( 'div' );
  tip.style.border = '10px solid transparent';
  tip.style.width = '0px';
  tip.style.borderTopColor = 'black';
  tip.style.position = 'absolute';
  tip.style.top = '100%';
  tip.style.left = '50%';
  tip.style.transform = 'translate( -50%, 0% )';
  d1.appendChild( tip );

  ev.target.appendChild( d1 );
}

// Replace to remove event listeners
var old_element = document.getElementById("qwe");
var new_element = old_element.cloneNode(true);
old_element.parentNode.replaceChild(new_element, old_element);


document.getElementById( 'qwe' ).addEventListener( 'click', addPopup );
