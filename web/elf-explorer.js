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
