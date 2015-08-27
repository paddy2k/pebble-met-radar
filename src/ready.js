Pebble.addEventListener('ready',
  function(e) {   
    setTimeout(function(){
      // Request current position
      var message = {
        "READY" : 1
      };
      Pebble.sendAppMessage(
        message,
        function(){
          console.log('Ready Sent');
        },
        function(){
          console.log('Ready Failed');
          console.log('Sending Again');
          setTimeout(function(){
            Pebble.sendAppMessage(message);  
         }, 1000);       
       }
     );
   }, 1000);

   console.log('Pebble Ready');
 }
);