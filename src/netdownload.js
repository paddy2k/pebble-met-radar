var transferInProgress = false;

Pebble.addEventListener("ready", function(e) {
  console.log("NetDownload JS Ready");
// console.log(navigator.userAgent);
//   window.BlobBuilder = window.BlobBuilder || window.WebKitBlobBuilder;
//   console.log(typeof BlobBuilder);
//   console.log(typeof Blob);
//   console.log(typeof WebKitBlobBuilder);
  
//   var xhr = new XMLHttpRequest();
//   xhr.open('GET', 'http://www.met.ie/weathermaps/radar2/WEB_radar3_201503301000.png', true);
//   xhr.responseType = 'blob';
//   xhr.setRequestHeader("Referer", "http://www.met.ie/latest/rainfall_radar.asp");
  
//   xhr.onload = function(e) {
//     if (this.status == 200) { 
//       if(window.BlobBuilder){
//         console.log('GO BlobBuilder');
//         var bb              = new BlobBuilder ();
//         bb.append (request.response); // Note: not request.responseText
    
//         var blob            = bb.getBlob ('image/png');
//         var reader          = new FileReader ();
//         reader.onload       = function (zFR_Event) {
//             console.log(zFR_Event.target.result); 
//             $("body").prepend ('<p>New image: <img src="' + zFR_Event.target.result + '"></p>')
//         };
//       }
//       else if(window.Blob){
//         console.log('GO Blob');
//         // Note: .response instead of .responseText
//         var blob = new Blob([this.response], {type: 'image/png'});  
//         var url = URL.createObjectURL(blob);
//         console.log(url); 
//         $("body").prepend ('<p>New image: <img src="' + url + '"></p>');
//       }
//       else{
//         console.log('FAIL ERROR');
//       }
      
      
      
//     }
//   };
  
//   xhr.send();
});

Pebble.addEventListener("appmessage", function(e) {
  console.log("Got message: " + JSON.stringify(e));
  console.log(e.payload);
  
  if ('NETDL_URL' in e.payload) {
    if (transferInProgress == false) {
      transferInProgress = true;
      downloadBinaryResource(e.payload['NETDL_URL'], function(bytes) {
        transferImageBytes(bytes, e.payload['NETDL_CHUNK_SIZE'],
          function() { console.log("Done!"); transferInProgress = false; },
          function(e) { console.log("Failed! " + e); transferInProgress = false; }
        );
      },
      function(e) {
        console.log("Download failed: " + e); transferInProgress = false;
      });
    }
    else {
      console.log("Ignoring request to download " + e.payload['NETDL_URL'] + " because another download is in progress.");
    }
  }
});

function downloadBinaryResource(imageURL, callback, errorCallback) {
    var req = new XMLHttpRequest();
    console.log('GET: '+imageURL);
    req.open("GET", imageURL,true);
    req.responseType = "arraybuffer";
    req.onload = function(e) {
        console.log("loaded");
        var buf = req.response;
        console.log('STATUS: '+req.status);
        if(req.status == 200 && buf) {
            var byteArray = new Uint8Array(buf);
            var arr = [];
            for(var i=0; i<byteArray.byteLength; i++) {
                arr.push(byteArray[i]);
            }

            console.log("Downloaded file with " + byteArray.length + " bytes.");
            callback(arr);
        }
        else {
          errorCallback("Request status is " + req.status);
        }
    }
    req.onerror = function(e) {
      errorCallback(e);
    }
    req.send(null);
}

function transferImageBytes(bytes, chunkSize, successCb, failureCb) {
  var retries = 0;

  success = function() {
    console.log("Success cb=" + successCb);
    if (successCb != undefined) {
      successCb();
    }
  };
  failure = function(e) {
    console.log("Failure cb=" + failureCb);
    if (failureCb != undefined) {
      failureCb(e);
    }
  };

  // This function sends chunks of data.
  sendChunk = function(start) {
    var txbuf = bytes.slice(start, start + chunkSize);
    console.log("Sending " + txbuf.length + " bytes - starting at offset " + start);
    Pebble.sendAppMessage({ "NETDL_DATA": txbuf },
      function(e) {
        // If there is more data to send - send it.
        if (bytes.length > start + chunkSize) {
          sendChunk(start + chunkSize);
        }
        // Otherwise we are done sending. Send closing message.
        else {
          Pebble.sendAppMessage({"NETDL_END": "done" }, success, failure);
        }
      },
      // Failed to send message - Retry a few times.
      function (e) {
        if (retries++ < 3) {
          console.log("Got a nack for chunk #" + start + " - Retry...");
          sendChunk(start);
        }
        else {
          failure(e);
        }
      }
    );
  };

  // Let the pebble app know how much data we want to send.
  Pebble.sendAppMessage({"NETDL_BEGIN": bytes.length },
    function (e) {
      // success - start sending
      sendChunk(0);
    }, failure);

}