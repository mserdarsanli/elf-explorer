var exampleObjectFile = new Uint8Array([
EMBED_FILE_HERE
]);

function passFileToEmscripten( file ) {
  return new Promise( (resolve, reject) => {
    var arrayBuffer;
    var fileReader = new FileReader();
    fileReader.onload = function(event) {
        arrayBuffer = event.target.result;
    };
    fileReader.onloadend = function () {
        arr = new Uint8Array( arrayBuffer );
        emAddr = allocate( arr,  'i8', ALLOC_NORMAL );
        resolve( [ emAddr, arr.length ] );
    };
    fileReader.readAsArrayBuffer( file );
  } );
}

function replacePageWith( htmlContents ) {
  document.getElementsByTagName( 'html' )[0].innerHTML = htmlContents;
}

function useExampleObject() {
  emAddr = allocate( exampleObjectFile,  'i8', ALLOC_NORMAL );
  htmlContents = Module.ccall( 'run_with_buffer', 'string', ['number', 'number'], [emAddr, exampleObjectFile.length] );
  replacePageWith( htmlContents );
}

document.getElementById( 'drop-area' ).addEventListener( 'dragover', ev => {
  ev.stopPropagation();
  ev.preventDefault();
});

document.getElementById( 'hello-object-image' ).addEventListener( 'dragstart', ev => {
  ev.dataTransfer.setData( 'object-id', ev.target.id );
});
document.getElementById( 'hello-object-image' ).addEventListener( 'click', ev => {
  useExampleObject();
});
document.getElementById( 'hello-object-image' ).addEventListener( 'touchend', ev => {
  useExampleObject();
});

document.getElementById( 'drop-area' ).addEventListener( 'drop', function( ev ) {
  ev.stopPropagation();
  ev.preventDefault();

  if ( ev.dataTransfer.getData( 'object-id' ) == 'hello-object-image' ) {
    useExampleObject();
    return;
  }

  if ( ev.dataTransfer.items.length != 1 ) {
    console.error( 'Can drop one file only' );
    return;
  }

  let item = ev.dataTransfer.items[0];

  if ( item.kind != 'file' ) {
    console.error( 'Dropped item must be a file' );
    return;
  }

  passFileToEmscripten( item.getAsFile() ).then( ( [addr, len] ) => {
    var htmlContents = Module.ccall( 'run_with_buffer', 'string', ['number', 'number'], [addr, len] );
    replacePageWith( htmlContents );
  } );

});



function addPopup( ev ) {
  ev.stopPropagation();
  console.log( 'addpopup called with event: ', ev );

  ev.target.style.position = 'relative';

  d1 = document.createElement( 'div' );
  d1.addEventListener( 'click', function( ev ) {
    // Is there a better way to do this?
    ev.stopPropagation();
  } );
  d1.innerHTML = '<div style="text-align: center; font-weight: bold; font-size: large;">SHT_RELA</div><p>Relocation entries with addends</p>';
  d1.style.position = 'absolute';
  d1.style.width = '300px';
  d1.style.top = '0%';
  d1.style.left = '50%';
  d1.style.transform = 'translate(-50%, calc( -100% - 10px ) )';
  d1.style.backgroundColor = '#eeeeee';
  d1.style.border = '1px solid black';
  d1.style.padding = '5px';

  a = document.createElement( 'span' );
  a.innerText = 'X';
  a.style.fontWeight = 'bold';
  a.style.backgroundColor = '#dddddd';
  a.addEventListener( 'click', function( ev ) {
    ev.stopPropagation();
    ev.target.parentNode.parentNode.remove();
  } );
  a2 = document.createElement( 'div' );
  a2.style.textAlign = 'right';
  a2.appendChild( a );
  d1.insertBefore( a2, d1.childNodes[0] );

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

function testAddPopup() {
  document.getElementsByTagName( 'abbr' )[4].addEventListener( 'click', addPopup );
}
