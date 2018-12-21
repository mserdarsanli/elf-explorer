// Copyright 2018 Mustafa Serdar Sanli
//
// This file is part of ELF Explorer.
//
// ELF Explorer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ELF Explorer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ELF Explorer.  If not, see <https://www.gnu.org/licenses/>.


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



function escapeHtml(unsafe) {
    return unsafe
         .replace(/&/g, "&amp;")
         .replace(/</g, "&lt;")
         .replace(/>/g, "&gt;")
         .replace(/"/g, "&quot;")
         .replace(/'/g, "&#039;");
 }



function addPopup( ev, enum_name ) {

  let innerHTML = `<div style="text-align: center; font-weight: bold; font-size: large;">${enum_name}</div><p>${ escapeHtml( enum_info[ enum_name ] ) }</p>`;


  ev.stopPropagation();
  console.log( 'addpopup called with event: ', ev );

  ev.target.style.position = 'relative';

  d1 = document.createElement( 'div' );
  d1.addEventListener( 'click', function( ev ) {
    // Is there a better way to do this?
    ev.stopPropagation();
  } );
  d1.innerHTML = innerHTML;
  d1.classList.add( 'popup' );

  a = document.createElement( 'span' );
  a.innerText = 'X';
  a.classList.add( 'popup-close-button' );
  a.addEventListener( 'click', function( ev ) {
    ev.stopPropagation();
    ev.target.parentNode.parentNode.remove();
  } );
  a2 = document.createElement( 'div' );
  a2.style.textAlign = 'right';
  a2.appendChild( a );
  d1.insertBefore( a2, d1.childNodes[0] );

  tip = document.createElement( 'div' );
  tip.classList.add( 'popup-tip' );
  d1.appendChild( tip );

  ev.target.appendChild( d1 );
}
