# 智能卡基础实验

李则言 吕鑫 卫翔宇

---

## 实验原理和步骤

通过Arduino连接RFC522，尝试对不同的卡进行读取，分析其安全性，并测试在不同环境下的极限读卡距离。

## 硬件设备与接线

- 开发板： Arduino/Genuino Uno

- 读卡器： RFID-RFC522

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

### 白卡(卡片状)

#### 类型与安全性分析

为M1 1K卡，加密秘钥为默认秘钥，基本无安全性，为最原始的试验卡。

#### 读取测试

| cover | distance （cm） |
| ----- | ------------- |
| None  | 2.5           |
| 口袋    | 1.8           |
| 钱包    | 1.6           |
| 安全卡套  | 无             |



### 白卡(水滴状)

#### 类型与安全性分析

为M1 1K卡，加密秘钥为默认秘钥，基本无安全性，为最原始的试验卡。

#### 读取测试

| cover | distance （cm） |
| ----- | ------------- |
| None  | 2.0           |
| 口袋    | 1.3           |
| 钱包    | 0.9           |
| 安全卡套  | 无             |



### 支持闪付的银行卡

#### 类型与安全性分析

银行卡类型为==TODO==，存储的数据是经过一定处理的，需要查阅一定资料才能读取，安全性比较高。

#### 读取测试

| cover | distance （cm） |
| ----- | ------------- |
| None  | 1.8           |
| 口袋    | 1.4           |
| 钱包    | 0.9           |
| 安全卡套  | 无             |



### 身份证

#### 类型与安全性分析

身份证是==TODO==卡，存储的数据是经过加密的，安全性很高。

#### 读取测试

无法读取



### 校园一卡通

#### 类型与安全性分析

校园一卡通为高频卡，使用RFC522无法直接读取，安全性相对较高。

#### 读取测试

无法读取



### 紫荆公寓澡卡

#### 类型与安全性分析

澡卡为M1卡，更改了卡的默认秘钥，无法读取相关内存块中的信息，安全性较高，但低于校园卡。

#### 读取测试

==TODO==



### 北京市公交一卡通

#### 类型与安全性分析

北京公交卡是==TODO==卡，无法直接读取内存中的数据，安全性比较高。

#### 读取测试

| cover | distance （cm） |
| ----- | ------------- |
| None  | 0.9           |
| 口袋    | 0.5           |
| 钱包    | 0.2           |
| 安全卡套  | 无             |



### 天津市公交单程卡

#### 类型与安全性分析

天津公交卡是==TODO==卡，可以读取卡片内存中的部分数据，但无法进行写入，安全性尚可，相对北京交通一卡通安全性较低。

#### 读取测试

| cover | distance （cm） |
| ----- | ------------- |
| None  | 2.1           |
| 口袋    | 1.8           |
| 钱包    | 1.3           |
| 安全卡套  | 无             |

## 实验总结

​	总的来看市面上大部分的卡无法简单获取其中存储的信息，基本都需要一些对应的解析或者解密步骤才能读取。不同卡加密的复杂程度不同，一般越重要的卡加密程度最高，如身份证，而像澡卡这种使用人数较少，内部使用的卡便只采用最基本的加密方法。

​	在使用RFC522进行读取距离试验中，我们可以得到以下结论：

​	1.不同的卡所需读卡距离不同，这不仅和卡的种类有关，还与卡的外形构造等有关系。

​	2.一般来说钱包的读卡距离是要比放在口袋里的读卡距离要近，这比较符合直观想象，因为钱包比较厚，阻挡相对较多。

​	3.360安全卡套可以阻挡RFC522对于卡的读取，整体实验来看阻挡效果很好。