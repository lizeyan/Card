# 智能卡基础实验

李则言 吕鑫 卫翔宇

---

## 实验原理和步骤

## 硬件设备与接线

- 开发板： Arduino/Genuino Uno

- 读卡器： RFID-RC522

- 读卡器与开发板接线：

  | 读卡器接口 | 开发板接口         |
  | ----- | ------------- |
  | SDA   | Digital IO 10 |
  | SCK   | Digital IO 13 |
  | MOSI  | Digital IO 11 |
  | MISO  | Digital IO 12 |
  | GND   | GND           |
  | RST   | Digital IO 9  |
  | 3.3V  | 3V            |

## 实验程序设计

### 程序文件：

```
.
├── deprecated.h
├── dump_info.ino
├── MFRC522.cpp
├── MFRC522Debug.cpp
├── MFRC522Debug.h
├── MFRC522Extended.cpp
├── MFRC522Extended.h
├── MFRC522.h
├── MFRC522Hack.cpp
├── MFRC522Hack.h
└── require_cpp11.h
```

说明： 

- `dump_info.ino`为工程文件，其他文件都来自开源库[miguelbalboa/rfid](https://github.com/miguelbalboa/rfid/tree/e71a86b7e09daaa759c181a35f74146869376235)

### 主要代码

`dump_info.ino`

``` c++
#include <SPI.h>
#include "MFRC522.h"

constexpr uint8_t RST_PIN = 9;          // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
```



## 实验结果

### 白卡

#### 类型与安全性分析



#### 读取测试

| cover | distance （cm） |
| ----- | ------------- |
| None  |               |
| 口袋    |               |
| 钱包    |               |
| 安全卡套  | $+\infty$     |

### 支持闪付的银行卡

### 身份证

### 校园一卡通

### 紫荆公寓澡卡

### 北京市公交一卡通

### 天津市公交单程卡

## 实验总结

