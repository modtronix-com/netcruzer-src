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
    //to simplify the display and to shorten the amount of code used.

    //$('#AN0').html(window.location.pathname);
    if (window.location.pathname.indexOf("ajax.htm") != -1) {
        //don't use setInterval here without canceling the previous timer first.
        setTimeout("dataForAjaxHtm()", 250); //Set delay when AJAX data will be updated again.
    }
  });
}

//Function to show V and A
function showVA(data,name) {
  var decimalVal;
  var hexStr;
  
  hexStr = $(data).find(name).text();
  
  // convert hex number to decimal stringnumber to a hex string
  decimalVal = parseInt(hexStr,16);
  
  return decimalVal;
}

//Update AJAX data for power.htm page
function dataForPowerHtm() {
  $.get('power.xml', function(xmlcontents) 
  {
    $('#pwrV1').html(showVA(xmlcontents, 'pwrV1'));
    $('#pwrV2').html(showVA(xmlcontents, 'pwrV2'));
    $('#pwrV3').html(showVA(xmlcontents, 'pwrV3'));
    $('#pwrV4').html(showVA(xmlcontents, 'pwrV4'));
    $('#pwrV5').html(showVA(xmlcontents, 'pwrV5'));
    $('#pwrV6').html(showVA(xmlcontents, 'pwrV6'));
    $('#pwrV7').html(showVA(xmlcontents, 'pwrV7'));
    $('#pwrV8').html(showVA(xmlcontents, 'pwrV8'));
    $('#pwrV9').html(showVA(xmlcontents, 'pwrV9'));
    $('#pwrV10').html(showVA(xmlcontents, 'pwrV10'));
    $('#pwrV11').html(showVA(xmlcontents, 'pwrV11'));
    $('#pwrV12').html(showVA(xmlcontents, 'pwrV12'));
    $('#pwrA1').html(showVA(xmlcontents, 'pwrA1'));
    $('#pwrA2').html(showVA(xmlcontents, 'pwrA2'));
    $('#pwrA3').html(showVA(xmlcontents, 'pwrA3'));
    $('#pwrA4').html(showVA(xmlcontents, 'pwrA4'));
    $('#pwrA5').html(showVA(xmlcontents, 'pwrA5'));
    $('#pwrA6').html(showVA(xmlcontents, 'pwrA6'));
    $('#pwrA7').html(showVA(xmlcontents, 'pwrA7'));
    $('#pwrA8').html(showVA(xmlcontents, 'pwrA8'));
    $('#pwrA9').html(showVA(xmlcontents, 'pwrA9'));
    $('#pwrA10').html(showVA(xmlcontents, 'pwrA10'));
    $('#pwrA11').html(showVA(xmlcontents, 'pwrA11'));
    $('#pwrA12').html(showVA(xmlcontents, 'pwrA12'));
    $('#pwrTemp').html(showVA(xmlcontents, 'pwrTemp'));
    
    //TEST CODE..............
    //var asciiHex=0x5A;
    //var num=0x55A;
    //$('#pwrV1').html(hex2Dec(asciiHex,16));
    //$('#pwrV1').html(dec2Hex(num,4));
    

    if (window.location.pathname.indexOf("power.htm") != -1) {
        setTimeout("dataForPowerHtm()", 250); //Set delay when AJAX data will be updated again.
    }
  });
}


////////// General Functions //////////

/** Send command via POST, cmd=name-value array.
	Ex. cmdSend("oy1=1")
	Ex. cmdSend(['oy1',1])
*/
function cmdSend(cmd) {
  var cmdStr = cmd;
  if (cmd instanceof Array) {
	if (cmd.length == 2) {
	  cmdStr = cmd[0] + "=" + cmd[1];
	}
	else {
	  alert('cmdSend() - Not array with 2 elements!')
	  return;
	}
  }
  var url = "cmd.htm?"+cmdStr
  console.log('Requesting URL = "' + url + '"');
  $.post(url);
  //$.get("cmd.htm?"+cmdStr); //get uses cache, post NEVER uses cache! This causes problems when using get!
}


////////// Bit/Byte/Hex Functions //////////

/** Returns num, with given bit set. num=variable, bit=index of bit, value from 0-n.*/
function bitSet(num,bit) {
  return num | 1<<bit;
}

/** Returns num, with given bit cleared. num=variable, bit=index of bit, value from 0-n.*/
function bitClear(num,bit){
  return num & ~(1<<bit);
}

/** Test if bit is set, num=variable, bit=index of bit, value from 0-n */
function isBitSet(num,bit) {
var mask=1<<bit;
return num&mask;
}
function bitIsSet(n,b) {return isBitSet(n,b);}

/** Returns num, with given bit toggled. num=variable, bit=index of bit, value from 0-n.*/
function bitToggle(num,bit){
  return bitIsSet(num,bit)?bitClear(num,bit):bitSet(num,bit);
}

/** Convert decimal number to a "uppercase hex" string, with given size.
 * d=number,
 * size=lenght of returned string. Pad with leading '0' if required
 */
function dec2AscHex(d,size) {
  var s = "000000000" + Number(d).toString(16).toUpperCase();
  return s.substr(s.length-size);
}

/** convert hex(value OR string) to decimal, ex. hex2Dec("01FA")=506, hex2Dec(100)=256 */
function hex2Dec(h) {return parseInt(h,16);} 


////////// Array Functions //////////

/** Returns byte at given index in "Ascii Hex" string. s=string "2 character upper case hex", idx=index of byte, value from 0-n.
 * ex. ascHexGetB("01023A",1) returns 2.
 */
function ascHexGetB(s,idx) {
  var i=idx*2;
  if (s.length < (i+2))
	return 0;
  var ret=parseInt(s.substr(idx*2,2),16);
  return isNaN(ret)?0:ret;	//Ret 0 if NaN
}

/** Returns word at given index. Same as ascHexGetB(), but returns 16-bit word. */
function ascHexGetW(s,idx) {
  var i=idx*2;
  if (s.length < (i+4))
	return 0;
  var ret=parseInt(s.substr((idx*2)+2,2)+s.substr((idx*2),2),16);
  return isNaN(ret)?0:ret;	//Ret 0 if NaN
}


///// Stack flags /////
/* Ex.
 * var stk0=0x~#gsk0~;
 * if ((stk0&STK0_SSL_CLIENT)!==0)	//SSL client enabled
 *	 //Do something...
 */
var STK0_IP_GLEANING = 0x01;
var STK0_ICMP_SERVER = 0x02;
var STK0_ICMP_CLIENT = 0x04;
var STK0_HTTP2_SERVER = 0x08;
var STK0_SSL_SERVER = 0x10;
var STK0_SSL_CLIENT = 0x20;
var STK0_AUTO_IP = 0x40;

var STK1_DHCP_CLIENT = 0x01;
var STK1_DHCP_SERVER = 0x02;
var STK1_FTP_SERVER = 0x04;
var STK1_SMTP_CLIENT = 0x08;
var STK1_SNMP_SERVER = 0x10;
var STK1_SNMPV3_SERVER = 0x20;
var STK1_TFTP_CLIENT = 0x40;
var STK1_TELNET_SERVER = 0x80;

var STK2_ANNOUNCE = 0x01;
var STK2_DNS = 0x02;
var STK2_DNS_SERVER = 0x04;
var STK2_NBNS = 0x08;
var STK2_REBOOT_SERVER = 0x10;
var STK2_SNTP_CLIENT = 0x20;
var STK2_DYNAMICDNS_CLIENT = 0x40;

var STK3_BERKELEY_API = 0x01;
var STK3_ZEROCONF_LINK_LOCAL = 0x02;
var STK3_ZEROCONF_MDNS_SD = 0x04;

