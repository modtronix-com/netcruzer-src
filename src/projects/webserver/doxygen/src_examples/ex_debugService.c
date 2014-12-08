
/**
@example ex_debugService.c
This example shows how to implement two debug commands.
- The first one will output "Hello World" if "hw" is entered in the "Send Debug Text" box of the @ref devtools_nzUsbTerminal
- The second one will output our current IP address if "ip" is entered in the "Send Debug Text" box of the @ref devtools_nzUsbTerminal
*/
 
/**
 * Debug service routine
 */
void debugService(void) {

    // Other Code ............

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO Rewrite code for new debugging method using circular buffer!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //Process any received debug message.
    if (debugHasString()) {
        debugStrLen = strlen(debugGetString());
        switch(debugGetString()[0]) {
            case 'h':
                //'hw' - write "Hello World" debug message if "hw" debug message is received
                if (debugStrLen==2 && debugGetString()[1]=='w') {
                    debugRemoveString();
                    cbufPutString(CIRBUF_TX_DEBUG, "\nHello World");
                }
                break;
            case 'i':
                //'ip' - Print current IP address
                if (debugStrLen==2 && debugGetString()[1]=='p') {
                    debugRemoveString();
                    sprintf(debugTempBuf, "\nIP Address: %d.%d.%d.%d", AppConfig.MyIPAddr.v[0], AppConfig.MyIPAddr.v[1], AppConfig.MyIPAddr.v[2], AppConfig.MyIPAddr.v[3]);
                    cbufPutString(CIRBUF_TX_DEBUG, debugTempBuf);
                }
                break;
        }
    }
}
