/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 * 
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */

#include <SPI.h>
#include <String.h>
#include "MFRC522.h"

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

String command;
String lastCommand;
int cardStatus = 0;

/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    
    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
    command = "";
}

/**
 * Main loop.
 */
void loop() {
    mfrc522.PCD_Init(); // Init MFRC522 card
    if (!mfrc522.PICC_IsNewCardPresent())
    {
      if(cardStatus == 1)
      {
        Serial.println("LEAVE");
        cardStatus = 0;
      }
      return;
    }
    else
    {
      if(cardStatus == 0)
      {
        if ( ! mfrc522.PICC_ReadCardSerial())
          return;
        Serial.print("ARRIVAL");
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
        Serial.println();
        cardStatus = 1;
      }
    }
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
//    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    
    String commandName = "NULL";
    int appendLogInt = 0;
    int appendLogBit = 0;
    char tmpchar;
    if(Serial.available() > 0) // 串口有命令，准备接收
    {
        command = "";
        Serial.println("\nready to accept a command\n");
    }

    while(Serial.available() > 0){  
        tmpchar = Serial.read();//读串口第一个字节
        command += tmpchar;
    }  
    delay(100); 
    if(command.length() > 0)
    {
        command.trim();
        commandName = command.substring(0, command.indexOf(' '));
        if(commandName.equals("APPENDLOG"))
        {
          String appendLogStr = command.substring(command.indexOf(' '));
          appendLogStr.trim();
          appendLogInt = appendLogStr.substring(0, appendLogStr.indexOf(' ')).toInt();
          appendLogBit = appendLogStr.substring(appendLogStr.indexOf(' ')).toInt();
          
        }
    }
    else
    {
        commandName = "NULL";
        return ;
    }
    delay(100);
//    if(lastCommand.equals(command))
//    {
//      return;
//    }


    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
    {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7

    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }    
    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    Serial.println("int");
    Serial.println(appendLogInt);


    
    
    
    byte sector         = 1;
    byte blockAddr[]      = {4, 5, 6, 8, 9};
    byte blockAddrNow     = 0;
    byte dataBlock[]    = {
        0x00, 0x00, 0x00, 0x00, //  1,  2,   3,  4,
        0x00, 0x00, 0x00, 0x00, //  5,  6,   7,  8,
        0x00, 0x00, 0x00, 0x00, //  9, 10, 255, 12,
        0x00, 0x00, 0x00, 0x00  // 13, 14,  15, 16
    };
    dataBlock[0] = (byte)(appendLogInt);
    dataBlock[1] = (byte)(appendLogInt >> 8);
    dataBlock[2] = (byte)(appendLogInt >> 16);
    dataBlock[3] = (byte)(appendLogInt >> 24);

    
    
    if(commandName.equals("LOG"))
    {
     
        // Show the whole sector as it currently is
        Serial.println(F("Current data in sector:"));
        mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
        Serial.println();
    
        // Read data from the block
        Serial.print(F("Reading data from block ")); 
        Serial.print(blockAddr[blockAddrNow]);
        Serial.println(F(" ..."));
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr[blockAddrNow], buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        Serial.print(F("Data in block ")); 
        Serial.print(blockAddr[blockAddrNow]); 
        Serial.println(F(":"));
        dump_byte_array(buffer, 16); 
        Serial.println();
        Serial.println();

        lastCommand = command;
        
    }
    else if(commandName.equals("APPENDLOG"))
    {
        // Write data to the block
        Serial.print(F("Writing data into block ")); 
        Serial.print(blockAddr[blockAddrNow]);
        Serial.println(F(" ..."));
        dump_byte_array(dataBlock, 16); 
        Serial.println();
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }


        lastCommand = command;
    }
//  else if(commandName.equals("NULL"))
//  {
//    delay(100);
//    Serial.print(".");
//  }


        mfrc522.PICC_HaltA(); // Halt PICC 
        mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
        
//    if(!commandName.equals("NULL"))
//    {
//        // Halt PICC
//        mfrc522.PICC_HaltA();                                                                                     
//        // Stop encryption on PCD
//        mfrc522.PCD_StopCrypto1();
//    }
}
//
///**
// * Helper routine to dump a byte array as hex values to Serial.
// */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
