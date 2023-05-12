#ifndef GLOBAL_H_
#define GLOBAL_H_

#define MAX_CHARGE_V	158
#define MAX_CHARGE_A	130
#define TARGET_CHARGE_V	160
#define MIN_CHARGE_A	20
#define CAPACITY 180

extern bool in1, in2;
extern bool out1, out2, out3;

typedef struct
{
  unsigned char valid; //a token to store EEPROM version and validity. If it matches expected value then EEPROM is not reset to defaults //0
  float ampHours; //floats are 4 bytes //1
  float kiloWattHours; //5
  float packSizeKWH; //9
  unsigned short int  maxChargeVoltage; //21
  unsigned short int targetChargeVoltage; //23

  unsigned char maxChargeAmperage; //25
  unsigned char minChargeAmperage; //26
  unsigned char capacity; //27
  unsigned char debuggingLevel; //29
} EESettings;

extern EESettings settings;
extern float Voltage;
extern float Current;
extern float Power;
//extern unsigned long CurrentMillis;
extern int Count;

extern unsigned short int errorDoProcessing;
extern unsigned short int errorHandle;
#endif
