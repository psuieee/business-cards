/**
  ******************************************************************************
  * @file    ST25DVSensor.h
  * @author  MCD Application Team
  * @brief   Header file of NFC ST25SV sensor module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#ifndef _ST25DVSENSOR_H_
#define _ST25DVSENSOR_H_

#include "ST25DV_IO/st25dv_io.h"
#include "./libNDEF/NDEF_class.h"
#include "i2cWrapper.h"


class ST25DV {
  public:
    ST25DV(int32_t gpo, int32_t lpd, I2CWrapper *i2c) : st25dv_io(gpo, lpd, i2c), ndef(&st25dv_io) {}

    int begin();
    int begin(uint8_t *buffer, uint16_t bufferLength);
    int writeURI(char* protocol, char* uri, char* info, size_t length);
    int readURI(char* *s, size_t length);
    int writeText(char* text, char* iso_lang,  NDEF_Text_encoding_t encoding, size_t length);
    int readText(char* *, size_t length);
    //int writeUnabridgedURI(char* uri, char* info, size_t length);
    //int readUnabridgedURI(char* *s, size_t length);
    int writeSMS(char* phoneNumber, char* message, char* info, size_t length);
    int readSMS(char* *phoneNumber, char* *message, size_t length);
    //int writeGEO(char* latitude, char* longitude, char* info, size_t length);
    //int readGEO(char* *latitude, char* *longitude, size_t length);
    int writeEMail(char* emailAdd, char* subject, char* message, char* info);
    int readEMail(char* *emailAdd, char* *subject, char* *message);
    //int writeWifi(char* SSID, Ndef_Wifi_Authentication_t auth, Ndef_Wifi_Encryption_t enc, char* key);
    //int readWifi(sWifiTokenInfo *wifitoken);
    //int writeVcard(sVcardInfo vcard);
    //int readVcard(sVcardInfo *vcard);
    //int appendAAR(char* pkgName);
    //int appendBluetoothOOB(Ndef_Bluetooth_OOB_t bluetooth, char *recordId);
    //int writeMyApp(sMyAppInfo *pMyAppStruct);
    NDEF_TypeDef readNDEFType();
    NDEF *getNDEF();

  protected:
    NFCTAG_StatusTypeDef ST25DV_Init(void);
    void ST25DV_GPO_Init(void);
    void ST25DV_GPO_DeInit(void);
    uint8_t ST25DV_GPO_ReadPin(void);
    void ST25DV_LPD_Init(void);
    void ST25DV_LPD_DeInit(void);
    uint8_t ST25DV_LPD_ReadPin(void);
    void ST25DV_LPD_WritePin(uint8_t LpdPinState);
    void ST25DV_I2C_Init(void);
    void ST25DV_SelectI2cSpeed(uint8_t i2cspeedchoice);

    ST25DV_IO st25dv_io;
    NDEF ndef;
    uint8_t NfctagInitialized = 0;
};

#endif
