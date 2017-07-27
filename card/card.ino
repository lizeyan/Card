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
int ledStatus = 0; // 0 -- no led  1 -- red  2 -- green
int ledTime = 0;

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

    key.keyByte[0] = 0x4E; // 0x4E, 0x55, 0x87, 0xB8, 0x13, 0xDF
    key.keyByte[1] = 0x55;
    key.keyByte[2] = 0x87;
    key.keyByte[3] = 0xB8;
    key.keyByte[4] = 0x13;
    key.keyByte[5] = 0xDF;    

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    
    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
    command = "";
}

void loop() {
    delay(10);
    ledTime += 10;

    if(ledTime > 999)
    {
        digitalWrite(greenLed,LOW);
        digitalWrite(redLed,LOW);
    }

  
    mfrc522.PCD_Init(); // Init MFRC522 card
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        if(cardStatus == 1)
        {
            Serial.println("LEAVE");
            digitalWrite(greenLed,LOW);
            digitalWrite(redLed,LOW);
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
            Serial.print("ARRIVAL ");
            dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
            Serial.println();
            cardStatus = 1;
        }
    }
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
    
    String commandName = "NULL";
    unsigned long appendLogInt = 0;
    int appendLogBit = 0;
    unsigned long appendLogAmount = 0;
    String appendLogLocation = "";
    unsigned long smallMoneyInt1 = 0;
    unsigned long smallMoneyInt2 = 0;
    unsigned long smallMoneyInt3 = 0;
    unsigned long smallMoneyInt4 = 0;
    char tmpchar;
//    if(Serial.available() > 0) // 串口有命令，准备接收
//    {
//        command = "";
//    }
    command = "";

    if(Serial.available() <= 0) // 串口无命令
    {
        delay(100);
        ledTime += 100;
        return;
    }

    while(Serial.available() > 0){  
        tmpchar = Serial.read();//读串口第一个字节
        delay(1);
        ledTime += 1;
        if(tmpchar == '\r' || tmpchar == '\n')
            break;
        command += tmpchar;
    }  
//    if(lastCommand.equals(command))
//    {
//        return;
//    }
    
    delay(50);
    ledTime += 50; 
    if(command.length() > 0)
    {
        Serial.print("receive the command -- ");
        Serial.println(command);
        command.trim();
        commandName = command.substring(0, command.indexOf(' '));
        if(commandName.equals("APPENDLOG"))
        {
            String appendStr = command.substring(command.indexOf(' '));
            appendStr.trim();
            appendLogInt = Str2uint(appendStr.substring(0, appendStr.indexOf(' ')));
            String appendStr2 = appendStr.substring(appendStr.indexOf(' '));
            appendStr2.trim();
            appendLogBit = appendStr2.substring(0, appendStr2.indexOf(' ')).toInt();
            String appendStr3 = appendStr2.substring(appendStr2.indexOf(' '));
            appendStr3.trim();
            appendLogAmount = Str2uint(appendStr3.substring(0, appendStr3.indexOf(' ')));
            String appendStr4 = appendStr3.substring(appendStr3.indexOf(' '));
            appendStr4.trim();
            appendLogLocation = appendStr4;          
             
//            Serial.print("command:");
//            Serial.println(command);
//            Serial.print("commandName:");
//            Serial.println(commandName);
//            Serial.print("Time:");
//            Serial.println(appendLogInt);
//            Serial.print("Bit:");
//            Serial.println(appendLogBit);       
//            Serial.print("Amount:");
//            Serial.println(appendLogAmount); 
        }
        else if(commandName.equals("SMALLMONEY"))
        {
            String appendStr = command.substring(command.indexOf(' '));
            appendStr.trim();
            smallMoneyInt1 = Str2uint(appendStr.substring(0, appendStr.indexOf(' ')));
            String appendStr2 = appendStr.substring(appendStr.indexOf(' '));
            appendStr2.trim();
            smallMoneyInt2 = Str2uint(appendStr2.substring(0, appendStr2.indexOf(' ')));
        }
    }
    else
    {
        commandName = "NULL";
        return ;
    }




    // Show some details of the PICC (that is: the tag/card)
//    Serial.print(F("Card UID:"));
//    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
//    Serial.println();
//    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
//    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
    {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;
    
    byte sector         = 1;
    byte blockAddr[]      = {4, 5, 6, 8, 9};
    byte locAddr[]      = {12, 16, 20, 24, 28}; // 每个string放在单独的扇区中，分配2块，最多32字节，即32个字符
    byte blockAddrNowAddr = 32;
    byte smallWalletAddr = 36;
    byte smallBlock[]    = {
        0x00, 0x00, 0x00, 0x00, //  amount
        0x00, 0x00, 0x00, 0x00, //  +/-
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    
    
    byte blockAddrNow = 0;
    if(!commandName.equals("MAKENEWCARD") && !commandName.equals("SHOW"))
    {
        AuthenticateA(blockAddrNowAddr);
        
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddrNowAddr, buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Read() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
        blockAddrNow = buffer[0];
    }

    
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

    
    if(commandName.equals("ASKFORLOG"))
    {
        for(int i=0;i<5;i++)
        {   
            String tmplog = "LOG ";
            AuthenticateA(blockAddr[(blockAddrNow+4-i)%5]);
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr[(blockAddrNow+4-i)%5], buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Read() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            if(bytes2Loginfo(buffer, 16).equals("No log"))
            {
                Serial.println(tmplog); 
                continue;
            }
            tmplog += bytes2Loginfo(buffer, 16);

            AuthenticateA(locAddr[(blockAddrNow+4-i)%5]);
            
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(locAddr[(blockAddrNow+4-i)%5], buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Read() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            tmplog += bytes2Loc(buffer, 16);
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(locAddr[(blockAddrNow+4-i)%5] + 1, buffer, &size);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Read() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            tmplog += bytes2Loc(buffer, 16);
            Serial.println(tmplog);            
        }
        lastCommand = command;  
    }
    else if(commandName.equals("APPENDLOG"))
    {
        Authenticate(blockAddr[blockAddrNow]);
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
//        Serial.println("\nInfo:\n");

        lastCommand = command;
    }
    else if(commandName.equals("CLEAR"))
    {
        byte clearBlock[]    = {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };
        for(int i=0;i<5;i++)
        {
            Authenticate(blockAddr[i]);
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr[i], clearBlock, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Write() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            Authenticate(locAddr[i]);
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(locAddr[i], clearBlock, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Write() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
            status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(locAddr[i]+1, clearBlock, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("MIFARE_Write() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
        }
        Authenticate(32);
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(32, clearBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        Authenticate(smallWalletAddr);
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(smallWalletAddr, clearBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        lastCommand = command;
    }
    else if(commandName.equals("SHOW"))
    {
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
        lastCommand = command;
    }
    else if(commandName.equals("SMALLMONEY"))
    {
        AuthenticateA(smallWalletAddr);
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(smallWalletAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }          
        smallBlock[0] = (byte)(smallMoneyInt1);
        smallBlock[1] = (byte)(smallMoneyInt1 >> 8);
        smallBlock[2] = (byte)(smallMoneyInt1 >> 16);
        smallBlock[3] = (byte)(smallMoneyInt1 >> 24);
        smallBlock[4] = (byte)(smallMoneyInt2);
        smallBlock[5] = (byte)(smallMoneyInt2 >> 8);
        smallBlock[6] = (byte)(smallMoneyInt2 >> 16);
        smallBlock[7] = (byte)(smallMoneyInt2 >> 24);
        
        Authenticate(smallWalletAddr);
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(smallWalletAddr, smallBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }

        lastCommand = command;
    }
    else if(commandName.equals("SMALLQUERY"))
    {
        String tmpsmall = "SMALLANSWER ";
        AuthenticateA(smallWalletAddr);
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(smallWalletAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        tmpsmall += bytes2Smallinfo(buffer, 16);
        Serial.println(tmpsmall);
        lastCommand = command;
    }
    else if(commandName.equals("ACCESSACCEPTED"))
    {
        digitalWrite(redLed,LOW);
        digitalWrite(greenLed,HIGH);
        ledStatus = 2;
        ledTime = 0;
        lastCommand = command;

    }
    else if(commandName.equals("ACCESSDENIED"))
    {
        digitalWrite(greenLed,LOW);
        digitalWrite(redLed,HIGH);
        ledStatus = 1;
        ledTime = 0;
        lastCommand = command;
    }
    else if(commandName.equals("MAKENEWCARD"))
    {
        resetCard();
    }
    else if(commandName.equals("NEWKEY"))
    {
        showNewKey();
    }
    else
    {
        Serial.println("ERROR COMMAND ");
        Serial.print("command:");
        Serial.println(command);
        Serial.print("commandName:");
        Serial.println(commandName);
        Serial.print("Time:");
        Serial.println(appendLogInt);
        Serial.print("Bit:");
        Serial.println(appendLogBit);       
        Serial.print("Amount:");
        Serial.println(appendLogAmount); 
        lastCommand = command;
    }

    

    mfrc522.PICC_HaltA(); // Halt PICC 
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
        
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? "0" : "");
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
      // Serial.println(F("Authenticating using key A..."));
      status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, authnum, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) 
      {
          Serial.print(F("PCD_Authenticate() failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
          return;
      }    
      // Authenticate using key B
      // Serial.println(F("Authenticating again using key B..."));
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
      // Serial.println(F("Authenticating using key A..."));
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
    String str = "";
    String nostr = "No log";
    if(buffer[5] != 1)
        return nostr;
    unsigned long alint = 0;
    int albit = 0;
    unsigned long alamount = 0;
    for(int i=0;i<4;i++)
    {
        unsigned long value = buffer[i];
        unsigned long c = value<<(8*i);
        alint += c;
    }
    albit = buffer[4];
    for(int i=0;i<4;i++)
    {
        unsigned long value = buffer[8+i];
        unsigned long c = value<<(8*i);
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

String bytes2Loc(byte* buffer, byte bufferSize)
{
    String str = "";
    for(int i=0;i<bufferSize;i++)
    {
        int value = buffer[i];
        if(value == 0)
            break;
        char c = (char)(value);
        str += c;
    }
    return str;
}

String bytes2Smallinfo(byte* buffer, byte bufferSize)
{
    String res = "";
    for(int i=0;i<2;i++)
    {
        unsigned long alamount = 0;
        for(int j=0;j<4;j++)
        {
            unsigned long value = buffer[i * 4 + j];
            unsigned long c = value<<(8*j);
            alamount += c;
        }
        res += (String)alamount;
        res += " ";
    }
    
    return res;
}

unsigned long Str2uint(String str)
{
    unsigned long res = 0;
    unsigned long rt = 1;
    for(int i=0;i<str.length();i++)
    {
        char c = str.charAt(str.length()-i-1);
        int ci = (int(c)-48);
        res += ci * rt;
        rt *= 10;
    }
    return res;    
}

void resetCard()
{
    MFRC522::StatusCode status;
    byte newKeyBlock[]    = {
        0x4E, 0x55, 0x87, 0xB8, 
        0x13, 0xDF, 0xFF, 0x07,
        0x80, 0x69, 0x4E, 0x55,
        0x87, 0xB8, 0x13, 0xDF
    };
    MFRC522::MIFARE_Key defaultkey;
    defaultkey.keyByte[0] = 0xFF;
    defaultkey.keyByte[1] = 0xFF;
    defaultkey.keyByte[2] = 0xFF;
    defaultkey.keyByte[3] = 0xFF;
    defaultkey.keyByte[4] = 0xFF;
    defaultkey.keyByte[5] = 0xFF;

    byte zeroBlock[]    = {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };
    zeroBlock[0] = (byte)(59130615);
    zeroBlock[1] = (byte)(59130615 >> 8);
    zeroBlock[2] = (byte)(59130615 >> 16);
    zeroBlock[3] = (byte)(59130615 >> 24);
    zeroBlock[4] = (byte)(1203875873);
    zeroBlock[5] = (byte)(1203875873 >> 8);
    zeroBlock[6] = (byte)(1203875873 >> 16);
    zeroBlock[7] = (byte)(1203875873 >> 24);
    
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 36, &defaultkey, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("SMALL MONEY PCD_Authenticate() A failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }    
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 36, &defaultkey, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("SMALL MONEY PCD_Authenticate() B failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(36, zeroBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    
    for(int i=7;i<=43;i+=4)
    { 
        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, i, &defaultkey, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) 
        {
            Serial.print(F("PCD_Authenticate() A failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }    
        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, i, &defaultkey, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) 
        {
            Serial.print(F("PCD_Authenticate() B failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(i, newKeyBlock, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("Change key MIFARE_Write() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
    }   
    return;
}

void showNewKey()
{
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::MIFARE_Key keyA;
    MFRC522::MIFARE_Key keyB;
    for (byte i = 0; i < 6; i++) {
        keyA.keyByte[i] = 0x00;
    }
    keyB.keyByte[0] = 0x4E; // 0x4E, 0x55, 0x87, 0xB8, 0x13, 0xDF
    keyB.keyByte[1] = 0x55;
    keyB.keyByte[2] = 0x87;
    keyB.keyByte[3] = 0xB8;
    keyB.keyByte[4] = 0x13;
    keyB.keyByte[5] = 0xDF;
    MFRC522::StatusCode status;
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 43, &keyA, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("show new key A PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }  
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 43, &keyB, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print(F("show new key B PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(43, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("show new key MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }  

    dump_byte_array(buffer, size);

//    byte newKeyBlock[]    = {
//            0x4E, 0x55, 0x87, 0xB8, 
//            0x13, 0xDF, 0xFF, 0x07,
//            0x80, 0x69, 0x4E, 0x55,
//            0x87, 0xB8, 0x13, 0xDF
//        };

            byte newKeyBlock[]    = {
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0x07,
            0x80, 0x69, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF
        };
           status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(43, newKeyBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("show new key MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
//
//    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(40, buffer, &size);
//    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("show new key MIFARE_Read() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
//    }  
//
//    dump_byte_array(buffer, size);
    
    return;
}

void resetUID()
{
    MFRC522::StatusCode status;
    Authenticate(40);
    byte newKeyBlock[]    = { // 12 0D 78 0D  6A 08 04 00  62 63 64 65  66 67 68 69
            0x12, 0x0D, 0x78, 0x0D,
            0x6A, 0x08, 0x04, 0x00,
            0x62, 0x63, 0x64, 0x65,
            0x66, 0x67, 0x68, 0x69
        };
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(40, newKeyBlock, 16);
            if (status != MFRC522::STATUS_OK) {
                Serial.print(F("Change uid MIFARE_Write() failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
            }
    return;
}

