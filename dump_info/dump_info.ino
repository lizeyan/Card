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
int redLed = 4;
int greenLed = 2;

/**
 * Initialize.
 */
void setup() {
    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
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
    int appendLogAmount = 0;
    String appendLogLocation = "";
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
          String appendStr = command.substring(command.indexOf(' '));
          appendStr.trim();
          appendLogInt = appendStr.substring(0, appendStr.indexOf(' ')).toInt();
          String appendStr2 = appendStr.substring(appendStr.indexOf(' '));
          appendStr2.trim();
          appendLogBit = appendStr2.substring(0, appendStr2.indexOf(' ')).toInt();
          String appendStr3 = appendStr2.substring(appendStr2.indexOf(' '));
          appendStr3.trim();
          appendLogAmount = appendStr3.substring(0, appendStr3.indexOf(' ')).toInt();
          String appendStr4 = appendStr3.substring(appendStr3.indexOf(' '));
          appendStr4.trim();
          appendLogLocation = appendStr4;          

//          Serial.print("command:");
//          Serial.println(command);
//          Serial.print("int:");
//          Serial.println(appendLogInt);
//          Serial.print("Bit:");
//          Serial.println(appendLogBit);
//          Serial.print("Amount:");
//          Serial.println(appendLogAmount);
//          Serial.print("Location:");
//          Serial.println(appendLogLocation);
          
          
        }
    }
    else
    {
        commandName = "NULL";
        return ;
    }
    delay(100);
    if(lastCommand.equals(command))
    {
      return;
    }


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

    // In this sample we use the second sector,ei
    // that is: sector #1, covering block #4 up to and including block #7

    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;
    
    
    byte sector         = 1;
    byte blockAddr[]      = {4, 5, 6, 8, 9};
    byte locAddr[]      = {12, 16, 20, 24, 28}; // 每个string放在单独的扇区中，分配2块，最多32字节，即32个字符
    
    
    byte blockAddrNow     = 0;
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 32, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(32, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
    blockAddrNow = buffer[0];

    
    byte dataBlock[]    = {
        0x00, 0x00, 0x00, 0x00, //  timestamp
        0x00, 0x00, 0x00, 0x00, //  +/-
        0x00, 0x00, 0x00, 0x00, //  amount
        0x00, 0x00, 0x00, 0x00  //  location
    };
    dataBlock[0] = (byte)(appendLogInt);
    dataBlock[1] = (byte)(appendLogInt >> 8);
    dataBlock[2] = (byte)(appendLogInt >> 16);
    dataBlock[3] = (byte)(appendLogInt >> 24);
    dataBlock[4] = (byte)(appendLogBit);
    dataBlock[5] = 1; // 标记是否已有log
    dataBlock[8] = (byte)(appendLogAmount);
    dataBlock[9] = (byte)(appendLogAmount >> 8);
    dataBlock[10] = (byte)(appendLogAmount >> 16);
    dataBlock[11] = (byte)(appendLogAmount >> 24);

    Serial.println(appendLogInt);
    Serial.println(appendLogBit);
    Serial.println(appendLogAmount);
    Serial.println(appendLogLocation);
//    Serial.println(appendLogLocation.getBytes());
    String subLoc0 = "";
    String subLoc1 = "";
    String subLoc2 = "";
    if(appendLogLocation.length() < 17)
    {
      subLoc0 = appendLogLocation;
    }
    else
    {
      subLoc0 = appendLogLocation.substring(0,16);
      if(appendLogLocation.length() < 33)
      {
        subLoc1 = appendLogLocation.substring(16);
      }
      else
      {
        subLoc1 = appendLogLocation.substring(16,32);
        subLoc2 = appendLogLocation.substring(32);
      }
    }
    Serial.print("subLoc0:");
    Serial.println(subLoc0);
    Serial.print("subLoc1:");
    Serial.println(subLoc1);
    Serial.print("subLoc2:");
    Serial.println(subLoc2);

    
    byte locBlock0[]    = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    byte locBlock1[]    = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00 
    };
    byte locBlock2[]    = {
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00 
    };

    String2Bytes(subLoc0, locBlock0, 16);
    String2Bytes(subLoc1, locBlock1, 16);
    dump_byte_array(locBlock0, 16);
    dump_byte_array(locBlock1, 16);
    
    
    if(commandName.equals("ASKFORLOG"))
    {
        for(int i=0;i<5;i++)
        {   
            Serial.print(F("Reading data from block ")); 
            Serial.print(blockAddr[(blockAddrNow-i)%5]);
            Serial.println(F(" ..."));
            AuthenticateA(blockAddr[(blockAddrNow-i)%5]);
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr[(blockAddrNow-i)%5], buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Read() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            Serial.print(F("Data in block ")); 
            Serial.print(blockAddr[(blockAddrNow-i)%5]); 
            Serial.println(F(":"));
            dump_byte_array(buffer, 16); 
            Serial.println();
            Serial.println(); 
        }

//          Serial.println(F("Authenticating using key A..."));
//          status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
//          if (status != MFRC522::STATUS_OK) {
//              Serial.print(F("PCD_Authenticate() failed: "));
//              Serial.println(mfrc522.GetStatusCodeName(status));
//              return;
//          }
//          Serial.print(F("Reading data from block ")); Serial.print(4);
//          Serial.println(F(" ..."));
//          status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(4, buffer, &size);
//          if (status != MFRC522::STATUS_OK) {
//              Serial.print(F("MIFARE_Read() failed: "));
//              Serial.println(mfrc522.GetStatusCodeName(status));
//          }
//          Serial.print(F("Data in block ")); Serial.print(4); Serial.println(F(":"));
//          dump_byte_array(buffer, 16); Serial.println();
//          Serial.println();

        lastCommand = command;
        
    }
    else if(commandName.equals("APPENDLOG"))
    {
        Authenticate(blockAddr[blockAddrNow]);
      
        // Write data to the block
        Serial.print(F("Writing data into block ")); 
        Serial.print(blockAddr[blockAddrNow]);
        Serial.println(F(" ..."));
        Serial.println("dataBlock:");
        dump_byte_array(dataBlock, 16); 
        Serial.println();
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr[blockAddrNow], dataBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }

        Authenticate(locAddr[blockAddrNow]);

        
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(locAddr[blockAddrNow], locBlock0, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(locAddr[blockAddrNow]+1, locBlock1, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
      blockAddrNow = (blockAddrNow + 1) % 5;
      byte paraBlock[]    = {
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00
      };
      paraBlock[0] = blockAddrNow;
      Authenticate(32);
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(32, paraBlock, 16);
      if (status != MFRC522::STATUS_OK) {
          Serial.print(F("MIFARE_Write() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
      }


        
        Serial.println("\nInfo:\n");
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

        lastCommand = command;
    }
    else if(commandName.equals("ACCESSACCEPTED"))
    {
        digitalWrite(redLed,LOW);
        digitalWrite(greenLed,HIGH);
        delay(1000);
        digitalWrite(greenLed,LOW);
        delay(1000); 
    }
    else if(commandName.equals("ACCESSDENIED"))
    {
        digitalWrite(greenLed,LOW);
        digitalWrite(redLed,HIGH);
        delay(1000);
        digitalWrite(redLed,LOW);
        delay(1000); 
    }

    

    mfrc522.PICC_HaltA(); // Halt PICC 
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
        
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println();
}

void String2Bytes(String str, byte* buffer, byte bufferSize)
{
  for(int i=0;i< str.length();i++)
  {
    char tmpchar = str.charAt(i);
    buffer[i] = (byte)(tmpchar);
  }
}

void Authenticate(byte blocknum)
{
      MFRC522::StatusCode status;
      byte authnum = blocknum - (blocknum % 4) + 3;
      // Authenticate using key A
      Serial.println(F("Authenticating using key A..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, authnum, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) 
      {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }    
      // Authenticate using key B
      Serial.println(F("Authenticating again using key B..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, authnum, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) 
      {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }
}

void AuthenticateA(byte blocknum)
{
      MFRC522::StatusCode status;
      byte authnum = blocknum - (blocknum % 4) + 3;
      // Authenticate using key A
      Serial.println(F("Authenticating using key A..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, authnum, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) 
      {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }    
}

String bytes2String(byte* buffer, byte bufferSize)
{
    String str = "";
    for(int i=0;i<bufferSize;i++)
    {
        int value = buffer[i];
        char c = (char)(value);
        str += c;
    }
    return str;
}

String bytes2Loginfo(byte* buffer, byte bufferSize)
{
//    dataBlock[0] = (byte)(appendLogInt);
//    dataBlock[1] = (byte)(appendLogInt >> 8);
//    dataBlock[2] = (byte)(appendLogInt >> 16);
//    dataBlock[3] = (byte)(appendLogInt >> 24);
//    dataBlock[4] = (byte)(appendLogBit);
//    dataBlock[5] = 1; // 标记是否已有log
//    dataBlock[8] = (byte)(appendLogAmount);
//    dataBlock[9] = (byte)(appendLogAmount >> 8);
//    dataBlock[10] = (byte)(appendLogAmount >> 16);
//    dataBlock[11] = (byte)(appendLogAmount >> 24);

  
    String str = "";
    String nostr = "No log";
    if(buffer[5] != 1)
        return nostr;
    int alint = 0;
    int albit = 0;
    int alamount = 0;
    for(int i=0;i<4;i++)
    {
        int value = buffer[i];
        int c = value<<(8*i);
        alint += c;
    }
    albit = buffer[4];
    for(int i=0;i<4;i++)
    {
        int value = buffer[8+i];
        int c = value<<(8*i);
        alamount += c;
    }
    str += (String)alint;
    str += " ";
    str += (String)albit;
    str += " ";
    str += (String)alamount;
    str += " ";
    
    return str;
}

