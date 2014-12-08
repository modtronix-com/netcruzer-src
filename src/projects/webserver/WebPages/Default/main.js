//This library should eventually replace mchp.js. It uses jQuery.

//Update AJAX data for ajax.htm page
function dataForAjaxHtm() {
  //ajax GET request to load the data.xml into xmlcontents var
  $.get('data.xml', function(xmlcontents) 
  {
    //you can read jQuery docs to understand the syntax of the next line
    //but in summary you have a local object directly named localdynvar1 so you can access it using # prefix
    //.find() searches for an XML tag, which in our case are sys_led, analog0
    $('#sysLed').html($(xmlcontents).find('sys_led').text());
    $('#AN0').html($(xmlcontents).find('analog0').text());
    //obviouslly for large amounts of data or grouped/array format, you may want to use for...next here
    //to simplify the display and to shorten the amount of code used.  From here on, it's just programming techniques.

		//$('#AN0').html(window.location.pathname);
		if (window.location.pathname.indexOf("ajax.htm") != -1) {
				//don't use setInterval here without canceling the previous timer first.
				setTimeout("dataForAjaxHtm()", 250); //you can use setTimeout or setInterval to read the contents of the XML again in, ie, 5 seconds
		}
  });
}
