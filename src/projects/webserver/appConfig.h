/**
 * @brief           application Configuration
 * @file            appConfig.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 Compiler
 *
 * @section appConfig_desc Description
 *****************************************
 * Application configuration.
 *
 **********************************************************************
 * @section appConfig_lic Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2013-11-20, David H. (DH):
 *    - Initial version
 *********************************************************************/
#ifndef APPCONF_H
#define APPCONF_H

//Have to include these because they define various CFG_BLOCKs
#include "nz_appConfig.h"
#include "nz_ioPorts.h"
#include "nz_serDataPorts.h"
#include "cmd.h"


/////////////////////////////////////////////////
//  Application Configuration - documentation module
/** @addtogroup proj_webserver_conf
 * @section proj_webserver_appconf Application Configuration
 * The application configuration is stored in the external EEPROM of the SBC board.
 * It is stored in the CFG_STRUCT section of the EEPROM. The XEEMEM_USER_START_ADR and XEEMEM_USER_END_ADR
 * defines should be used to get the address range available for user data.
 *
 *
 * @section proj_webserver_appconf_memmap External EEPROM Memory Map
 * The memory map for the external EEPROM is:
 * <table>
 * <tr>
 * <td><b>0x0000-0x001F</b> (0-31)</td>
 * <td>CFG_INFO - Info, Magic Number, CRC... Used for checksum calculations, and to check if EEPROM contains valid data.</td>
 * </tr>
 * <tr>
 * <td><b>0x0020-0x03FF</b> (32-1023)</td>
 * <td>CFG_COPY - Copy of last written settings. Use write blocks of 32 bytes</td>
 * </tr>
 * <tr>
 * <td><b>0x0400-0x0FFF</b> (1024-4095)</td>
 * <td>CFG_STRUCT - Main Content.<br>In code this address range is defined by the XEEMEM_CFG_STRUCT_START_ADR and XEEMEM_CFG_STRUCT_END_ADR defines.</td>
 * </tr>
 * <tr>
 * <td><b>0x1000-0x1FFF</b> (4096-8191)</td>
 * <td><b>User EEPROM</b> space, can be used by user application to store non volatile data.<br>In code this address range is defined by the XEEMEM_USER_START_ADR and XEEMEM_USER_END_ADR defines.</td>
 * </tr>
 * </tr></table>
 *
 *
 * @section proj_webserver_appconf_details Details
 * The default configuration is stored in the cfgStructHexfile and cfgStruct
 * structures in the appConfig.c file. Both these strucures should be kept identical! When making a
 * change to one, do the same to the other!
 * - The <b>cfgStructHexfile</b> structure places the default configuration in the generated
 *   hex file at byte address 0x1000000. This is outside the target device's FLASH program memory,
 *   and will not be used by a PIC programmer. This data is only used by the @ref devtools_nzUsbBootloader
 *   The @ref devtools_nzUsbBootloader takes this data from the hex file, and programs it onto the external FLASH
 *   of the target board when doing a firmware upgrade. This default configuration data is then
 *   used by the target board to update it's application configuration data.
 * - The <b>cfgStruct</b> structure places the default configuration in the target device's FLASH
 *   program memory. It only happens when the CFG_STRUCT_IN_CODE macro is defines. This has to
 *   be done when the firmware is NOT programmed with the @ref devtools_nzUsbBootloader, but with
 *   a PIC programmer. The reason is that the cfgStructHexfile is only programmed into the target's
 *   external EEPROM when the @ref devtools_nzUsbBootloader is used. When a PIC programmer is used,
 *   the cfgStruct is required. It uses additional FLASH program memory of the target! When the
 *   CFG_STRUCT_IN_CODE macro is defined, the firmware will update the application configuration data
 *   (in external EEPROM) with the contents of the cfgStruct structure.
 *
 * @section proj_webserver_appconf_modify Modifying AppConfig
 * When upgrading firmware, the old configuration data should be preserved. To do this, it is very important
 * that if the config structures are changed in new firmware, changes are ONLY made to back of structure.
 * The upgrade process will work as follows:
 * - Find current copy of CFG_STRUCT in external FLASH. Remember it as "Previous CFG_STRUCT"
 * - Overwrite external EEPROM with new CFG_STRUCT (contained in new firmware)
 * - Iterate through all CFG_BLOCKs in "Previous CFG_STRUCT". For each one found, search for the corresponding
 *      CFG_BLOCK in new CFG_STRUCT (external EEPROM). When found, copy data from "Previous" CFG_BLOCK to new
 *      one in external EEPROM.
 *
 */


/////////// Network Default Defines /////////////////////
#if !defined(DEFAULT_WIFI_SSID)
#define DEFAULT_WIFI_SSID                       { 0 }
#endif


/////////// WiFi Default Defines /////////////////////
#if !defined(DEFAULT_WIFI_SSID)
#define DEFAULT_WIFI_SSID                       ( 0 )
#endif
#if !defined(DEFAULT_WIFI_SSID_LENGTH)
#define DEFAULT_WIFI_SSID_LENGTH                ( 0 )
#endif
#if !defined(DEFAULT_WIFI_SECURITY_MODE)
#define DEFAULT_WIFI_SECURITY_MODE              ( 0 )
#endif
#if !defined(DEFAULT_WIFI_SECURITY_KEY)
#define DEFAULT_WIFI_SECURITY_KEY               { 0 }
#endif
#if !defined(DEFAULT_WIFI_SECURITY_KEY_LENGTH)
#define DEFAULT_WIFI_SECURITY_KEY_LENGTH        ( 0 )
#endif
#if !defined(DEFAULT_WIFI_WEP_KEY_INDEX)
#define DEFAULT_WIFI_WEP_KEY_INDEX              ( 0 )
#endif
#if !defined(DEFAULT_WIFI_DATA_VALID)
#define DEFAULT_WIFI_DATA_VALID                 ( 0 )
#endif
#if !defined(DEFAULT_WIFI_NETWORK_TYPE)
#define DEFAULT_WIFI_NETWORK_TYPE               ( 0 )
#endif
#if !defined(DEFAULT_WIFI_SAVED_SEC_INFO)
#define DEFAULT_WIFI_SAVED_SEC_INFO             ( 0 )
#endif



/////////// Variables ///////////////////////////
// Structure to contain a MAC address
typedef struct __attribute__((__packed__))
{
    BYTE v[6];
} MAC_ADDR;

/** Definition to represent an IP address */
#define IP_ADDR        DWORD_VAL

// Address structure for a node
typedef struct __attribute__((__packed__))
{
    IP_ADDR     IPAddr;
    MAC_ADDR    MACAddr;
} NODE_INFO;


/**
 * DON'T CHANGE! Ensure these structures contain all members of origianl APP_CONFIG that is
 * commented out in "StackTsk.h" file! Some changes had to be made (to orginal in StackTsk.h)
 * so that all members are on 16-bit boundaries, like "Flags" for example.
 * Structure below consists out of copies of CFG_BLOCK_xxx structures, that are made available
 * in RAM for quick access. The "reserve[]" members of the CFG_BLOCK_xxx structures are NOT
 * included!
 * Consists of following CFG_BLOCK_xxx structures:
 * - CFG_BLOCK_NET
 * - CFG_BLOCK_WIFI
 * - CFG_BLOCK_SNMP
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    /////////////////////////////////////////////////
    /// Network Settings. !!!!!!!!!!  Ensure it is identical to CFG_BLOCK_NET, except for 'reserve[]' array  !!!!!!!!!!
    IP_ADDR         MyIPAddr;               ///< IP address
    IP_ADDR         MyMask;                 ///< Subnet mask
    IP_ADDR         MyGateway;              ///< Default Gateway
    IP_ADDR         PrimaryDNSServer;       ///< Primary DNS Server
    IP_ADDR         SecondaryDNSServer;     ///< Secondary DNS Server
    IP_ADDR         DefaultIPAddr;          ///< Default IP address
    IP_ADDR         DefaultMask;            ///< Default subnet mask
    BYTE            NetBIOSName[16];        ///< NetBIOS name
    struct
    {
        unsigned char bIsDHCPEnabled : 1;
        unsigned char bInConfigMode : 1;
        unsigned int bFill : 14;        ///< Fill to ensure flags take up a 16-bit WORD
    } Flags;                            ///< Flag structure
    MAC_ADDR    MyMACAddr;              ///< Application MAC address

#if defined(WF_CS_TRIS)
    /////////////////////////////////////////////////
    /// WiFi Settings. !!!!!!!!!!  Ensure it is identical to CFG_BLOCK_WIFI, except for 'reserve[]' array  !!!!!!!!!!
    BYTE        MySSID[32];             ///< Wireless SSID (if using MRF24WB0M)
    BYTE        SsidLength;             ///< number of bytes in SSID
    BYTE        SecurityMode;           ///< WF_SECURITY_OPEN or one of the other security modes
    BYTE        SecurityKey[64];        ///< WiFi Security key, or passphrase.
    BYTE        SecurityKeyLength;      ///< number of bytes in security key (can be 0)
    BYTE        WepKeyIndex;            ///< WEP key index (only valid for WEP)
    BYTE        dataValid;
    BYTE        networkType;
    #if defined(EZ_CONFIG_STORE) 		///< WLAN configuration data stored to NVM
    BYTE        saveSecurityInfo;       ///<  Save 32-byte PSK
    #endif
#endif

//Disable "if defined(..)" check, and always have it part of APP_CONFIG
//#if defined(STACK_USE_SNMP_SERVER) || defined(STACK_USE_SNMPV3_SERVER)
    /////////////////////////////////////////////////
    /// SNMP Settings. !!!!!!!!!!  Ensure it is identical to CFG_BLOCK_SNMP, except for 'reserve[]' array  !!!!!!!!!!

    ///  SNMPv2C Read community names
    ///  SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
    //BYTE readCommunity[SNMP_MAX_COMMUNITY_SUPPORT][SNMP_COMMUNITY_MAX_LEN+1];
    BYTE readCommunity[3][8+1];

    ///  SNMPv2C Write community names
    ///  SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
    //BYTE writeCommunity[SNMP_MAX_COMMUNITY_SUPPORT][SNMP_COMMUNITY_MAX_LEN+1];
    BYTE writeCommunity[3][8+1];

    UINT32 SnmpEngineBootRcrd;
//#endif

} APP_CONFIG;


/**
 * Network Settings. Ensure it is identical to "Network Settings" in APP_CONFIG
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    IP_ADDR         MyIPAddr;               ///< IP address (offset 0 - 0x00)
    IP_ADDR         MyMask;                 ///< Subnet mask (offset 4 - 0x04)
    IP_ADDR         MyGateway;              ///< Default Gateway (offset 8 - 0x08)
    IP_ADDR         PrimaryDNSServer;       ///< Primary DNS Server (offset 12 - 0x0C)
    IP_ADDR         SecondaryDNSServer;     ///< Secondary DNS Server (offset 16 - 0x10)
    IP_ADDR         DefaultIPAddr;          ///< Default IP address (offset 20 - 0x14)
    IP_ADDR         DefaultMask;            ///< Default subnet mask (offset 24 - 0x18)
    BYTE            NetBIOSName[16];        ///< NetBIOS name (offset 28 - 0x1C)
    struct
    {
        unsigned char bIsDHCPEnabled : 1;
        unsigned char bInConfigMode : 1;
        unsigned int  bFill : 14;           ///< Fill to ensure flags take up a 16-bit WORD
    } Flags;                                ///< Flag structure (offset 44 - 0x2C)
    MAC_ADDR        MyMACAddr;              ///< Application MAC address (offset 46 - 0x2E)
    BYTE            reserve[8];             ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_NET;

/**
 * WiFi Settings. Ensure it is identical to "WiFi Settings" in APP_CONFIG
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    BYTE        MySSID[32];             ///< Wireless SSID (if using MRF24WB0M)
    BYTE        SsidLength;             ///< number of bytes in SSID
    BYTE        SecurityMode;           ///< WF_SECURITY_OPEN or one of the other security modes
    BYTE        SecurityKey[64];        ///< WiFi Security key, or passphrase.
    BYTE        SecurityKeyLength;      ///< number of bytes in security key (can be 0)
    BYTE        WepKeyIndex;            ///< WEP key index (only valid for WEP)
    BYTE        dataValid;
    BYTE        networkType;
    BYTE        saveSecurityInfo;       ///< Save 32-byte PSK
    BYTE        reserve[21];            ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_WIFI;

/**
 * SNMP Settings. Ensure it is identical to "SNMP Settings" in APP_CONFIG
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    /// SNMPv2C Read community names
    /// SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
    BYTE    readCommunity[3][8+1];

    /// SNMPv2C Write community names
    /// SNMP_COMMUNITY_MAX_LEN (8) + 1 null termination byte
    BYTE    writeCommunity[3][8+1];

    UINT32  SnmpEngineBootRcrd;

    BYTE            reserve[2];         ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_SNMP;



#if defined(STACK_USE_SNMP_SERVER) || defined(STACK_USE_SNMPV3_SERVER)
    #if (SNMP_MAX_COMMUNITY_SUPPORT != 3)
    #error "CFG_STRUCT assumes SNMP_MAX_COMMUNITY_SUPPORT = 3. It is NOT! Code must be changed"
    #endif

    #if (SNMP_COMMUNITY_MAX_LEN != 8)
    #error "CFG_STRUCT assumes SNMP_COMMUNITY_MAX_LEN = 8. It is NOT! Code must be changed"
    #endif
#endif


/**
 * Block from String Table
 */
typedef struct __attribute__((__packed__))
{
    BYTE    buf[8];
} STRING8_BLOCK;

/**
 * String Table. The string table consists out of blocks, each 8 bytes long. Each entry can either be a 'Header', or character.
 * A 'Header' has a value between 0x80 to 0xff, the bottom 7 bits give length of string to follow. Strings are NOT NULL terminated!
 * - A string can occupy a single block if it is 7 characters or less (header byte, string). Format will be:
 *   [0x80 | size] [0] [1] [2] [3] [4] [5] [6]
 * - If it is longer than 7 bytes, multiple blocks are required. Maximum length is 127 characters.
 *   [0x80 | size] [0] [1] ...... [size-1]
 * - A service routine can occasionally compact the string table if required.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    WORD        info;
    char        str8[31][8];

    BYTE        reserve[2];            ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_STRING_31X8;
#define CFG_BLOCK_STRINGS_VERSION 1        ///< Version 1

/** Maximum lenght of Username, including NULL termination. Can be max 11 character. Do NOT change this! */
#define USERNAME_SIZE   12

/** Maximum lenght of Password, including NULL termination. Can be max 11 character. Do NOT change this! */
#define PASSWORD_SIZE   12

/**
 * General Settings.
 * Ensure all WORDs and DWORDs are on 16-bit boundaries, must be done seeing that this struct can be located in internal FLASH!
 */
typedef struct __attribute__((__packed__))
{
    struct
    {
        unsigned char bTest : 1;
        unsigned int  bFill : 15;           ///< Fill to ensure flags take up a 16-bit WORD
    } Flags;                                ///< Flag structure

    BYTE    username[USERNAME_SIZE];
    BYTE    password[PASSWORD_SIZE];

    BYTE    reserve[34];            ///< Reserve, and get block size on 32 byte multiple. Ensure size is 4 less then 32 byte multiple, seeing that each CFG_BLOCK_XXX is preceded by a CFG_BLOCK_HDR (4 bytes long). Use cfgInfo debug message to confirm!
} CFG_BLOCK_GENERAL;


/**
 * Main structure containing all application configuration data. Will be placed in:
 *- When CFG_STRUCT_IN_CODE is defined, CFG_STRUCT will be in program FLASH.
 *- When CFG_STRUCT_IN_CODE is NOT defined, CFG_STRUCT will be in external FLASH. When the project is compiled,
 *  CFG_STRUCT is placed in hex file at byte address 0x1000000. This hex file is used to update the firmware
 *  of the board with the @ref devtools_nzUsbBootloader application. This bootloader will place this structure
 *  in the external FLASH.
 *
 * The first couple of structures are mirrored in the APP_CONFIG structure. This structure
 * resides in RAM, which allows fast access.
 */
//typedef struct __attribute__((aligned(2), packed)) _CFG_STRUCT
typedef struct __attribute__((packed))
{
    //The first entry of every CFG_STRUCT MUST be CFG_STRUCT_INFO!
    //CFG_BLOCK_HDR         infoHdr;        // Header is part of CFG_STRUCT_INFO!!!!
    CFG_BLOCK_INFO         info;

    // Network Configuration
    CFG_BLOCK_HDR           netHdr;         // Header
    CFG_BLOCK_NET           net;            ///< Network Settings

    // WiFi Configuration
    CFG_BLOCK_HDR           wifiHdr;        // Header
    CFG_BLOCK_WIFI          wifi;           ///< WiFi Settings

    // SNMP Configuration
    CFG_BLOCK_HDR           snmpHdr;        // Header
    CFG_BLOCK_SNMP          snmp;           ///< SNMP Settings

    // General Configuration
    CFG_BLOCK_HDR           genHdr;         // Header
    CFG_BLOCK_GENERAL       gen;            ///< General Settings

    // General Configuration
    CFG_BLOCK_HDR           net2Hdr;        // Header
    CFG_BLOCK_NET2          net2;           ///< Network 2 Settings

    // "Serial Data Port" Configuration
    CFG_BLOCK_HDR           ser0to7Hdr;    // Header
    CFG_BLOCK_SER_0TO7      ser0to7;       ///< "Serial Data Port" Settings

    // "Serial Data Port" Configuration
    CFG_BLOCK_HDR           ser8to15Hdr;   // Header
    CFG_BLOCK_SER_8TO15     ser8to15;      ///< "Serial Data Port" Settings

    // "Serial Data Port" Configuration
    CFG_BLOCK_HDR           ser16to23Hdr;   // Header
    CFG_BLOCK_SER_16TO23    ser16to23;      ///< "Serial Data Port" Settings

    // "Serial Data Port" Configuration
    CFG_BLOCK_HDR           ser24to31Hdr;   // Header
    CFG_BLOCK_SER_24TO31    ser24to31;      ///< "Serial Data Port" Settings

    // Microcontroller (UC) Port Configuration
    CFG_BLOCK_HDR           ucPortHdr;      // Header
    CFG_BLOCK_UC66_PORT     ucPort;         ///<

    // IO Port (SBC Port) Configuration
    CFG_BLOCK_HDR           ioPortHdr;     // Header
    CFG_BLOCK_SBC66_PORT    ioPort;

    // String Table
    CFG_BLOCK_HDR           str8aHdr;       // Header
    CFG_BLOCK_STRING_31X8   str8a;          ///< Block with 31 x 8 character long strings


    /////////////////////////////////////////////////
    //Constant TABLE_BLOCKs start from here!
    //VERY IMPORTANT!!! Update CFG_STRUCT_CONFSIZE define to give size of CFG_BLOCK part of CFG_STRUCT!!

    /// Properties of each "Microcontroller (UC) Port". Examples to get a Ports UCPORT_PROP structure:
    TABLE_BLOCK_HDR             ucPortPropHdr;  // Header
    TABLE_BLOCK_UC66_PORT       ucPortProp;

    /// IO Port (ports of SBC) properties
    TABLE_BLOCK_HDR             ioPortPropHdr;     // Header
    TABLE_BLOCK_SBC66_PORT      ioPortProp;
} CFG_STRUCT;
// END CFG_STRUCT defined

//Size of the CFG_BLOCK part of CFG_STRUCT. The last part of CFG_STRUCT contains TABLE_BLOCK,
//and is not stored in non volatile memory.
#define CFG_STRUCT_CONFSIZE offsetof(CFG_STRUCT, ucPortPropHdr)


/////////// Global variables ////////////////////
#ifndef THIS_IS_APPCONFIG_C
    //extern const CFG_STRUCT cfgStruct;    //Don't use const, on XC16 doesn't work any more. Compiler optimization assumes CFG_STRUCT never changes, and causes trouble. CFG_STRUCT does change!
    extern CFG_STRUCT       cfgStruct __attribute__ ((space(auto_psv)));

    extern APP_CONFIG       AppConfig;
#endif


/////////// Function Prototypes /////////////////
void appConfInit(void);

#endif
