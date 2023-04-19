#ifndef CHADEMO_H_
#define CHADEMO_H_
//#include <Arduino.h>
#include "globals.h"
//#include <due_can.h>
//#include <cstdint>

enum CHADEMOSTATE
{
  STARTUP,                      //0
  SEND_INITIAL_PARAMS,          //1
  WAIT_FOR_EVSE_PARAMS,         //2
  SET_CHARGE_BEGIN,             //3
  WAIT_FOR_BEGIN_CONFIRMATION,  //4
  CLOSE_CONTACTORS,             //5
  RUNNING,                      //6
  CEASE_CURRENT,                //7
  WAIT_FOR_ZERO_CURRENT,        //8
  OPEN_CONTACTOR,               //9
  FAULTED,                      //10
  STOPPED,                      //11
  LIMBO                         //12
};

typedef struct
{
  unsigned char supportWeldCheck;
  unsigned short int availVoltage : 16;
  unsigned char availCurrent;
  unsigned short int thresholdVoltage : 16; //evse calculates this. It is the voltage at which it'll abort charging to save the battery pack in case we asked for something stupid
} EVSE_PARAMS;

typedef struct
{
  unsigned short int presentVoltage : 16;
  unsigned char presentCurrent;
  unsigned char status;
  unsigned short int remainingChargeSeconds : 16;
} EVSE_STATUS;

typedef struct
{
    unsigned short int targetVoltage; //what voltage we want the EVSE to put out
    unsigned char targetCurrent; //what current we'd like the EVSE to provide
    unsigned char remainingKWH; //report # of KWh in the battery pack (charge level)
    unsigned char battOverVolt : 1; //we signal that battery or a cell is too high of a voltage
    unsigned char battUnderVolt : 1; //we signal that battery is too low
    unsigned char currDeviation : 1; //we signal that measured current is not the same as EVSE is reporting
    unsigned char battOverTemp : 1; //we signal that battery is too hot
    unsigned char voltDeviation : 1; //we signal that we measure a different voltage than EVSE reports
    unsigned char chargingEnabled : 1; //ask EVSE to enable charging
    unsigned char notParked : 1; //advise EVSE that we're not in park.
    unsigned char chargingFault : 1; //signal EVSE that we found a fault
    unsigned char contactorOpen : 1; //tell EVSE whether we've closed the charging contactor
    unsigned char stopRequest : 1; //request that the charger cease operation before we really get going
} CARSIDE_STATUS;

//The IDs for chademo comm - both carside and EVSE side so we know what to listen for
//as well.
#define CARSIDE_BATT_ID			0x100
#define CARSIDE_CHARGETIME_ID	0x101
#define CARSIDE_CONTROL_ID		0x102

#define EVSE_PARAMS_ID			0x108
#define EVSE_STATUS_ID			0x109

#define CARSIDE_FAULT_OVERV		1 //over voltage
#define CARSIDE_FAULT_UNDERV	2 //Under voltage
#define CARSIDE_FAULT_CURR		4 //current mismatch
#define CARSIDE_FAULT_OVERT		8 //over temperature
#define CARSIDE_FAULT_VOLTM		16 //voltage mismatch

#define CARSIDE_STATUS_CHARGE	1 //charging enabled
#define CARSIDE_STATUS_NOTPARK	2 //shifter not in safe state
#define CARSIDE_STATUS_MALFUN	4 //vehicle did something dumb
#define CARSIDE_STATUS_CONTOP	8 //main contactor open
#define CARSIDE_STATUS_CHSTOP	16 //charger stop before even charging

#define EVSE_STATUS_CHARGE		1 //charger is active
#define EVSE_STATUS_ERR			2 //something went wrong
#define EVSE_STATUS_CONNLOCK	4 //connector is currently locked
#define EVSE_STATUS_INCOMPAT	8 //parameters between vehicle and charger not compatible
#define EVSE_STATUS_BATTERR		16 //something wrong with battery?!
#define EVSE_STATUS_STOPPED		32 //charger is stopped

class CHADEMO
{
  public:
    CHADEMO();
    void setDelayedState(int newstate, unsigned short int delayTime, unsigned long CurrentMillis);
    CHADEMOSTATE getState();
    void setTargetAmperage(unsigned char t_amp);
    void setTargetVoltage(unsigned short int t_volt);
    void loop(unsigned long CurrentMillis);
    void doProcessing();
    void handleCANFrame(unsigned long CurrentMillis, unsigned int receiveID);
    void setChargingFault();
    void setBattOverTemp();

    //Teodora added
    void checkChargingState();
    unsigned long ChargeTimeRefSecs;
    void ResetChargeState();
    void updateTargetAV();

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    unsigned long CurrentMillis;

    unsigned char bChademo10Protocol; //can we use 1.0 protocol?
    unsigned char askingAmps;

    unsigned int insertionTime;

    CARSIDE_STATUS carStatus;
    EVSE_PARAMS evse_params;
    EVSE_STATUS evse_status;
    CHADEMOSTATE chademoState;

    bool sendBatt;
    bool sendStatus;
    bool sendTime;

    unsigned char faults;
    unsigned char status;

    //these need to be accessed quickly in tight spots so they're public in an attempt at efficiency
    unsigned char bChademoMode; //accessed but not modified in ISR so it should be OK non-volatile
    unsigned char bChademoSendRequests; //should we be sending periodic status updates?
    volatile unsigned char bChademoRequest;  //is it time to send one of those updates?

  protected:
  private:
      unsigned char bStartedCharge; //we have started a charge since the plug was inserted. Prevents attempts to restart charging if it stopped previously
      //unsigned char bChademo10Protocol; //can we use 1.0 protocol?
    //target values are what we send with periodic frames and can be changed.
    //TEODORA: i need this askingAmps so it could not be private
      //unsigned char askingAmps; //how many amps to ask for. Trends toward targetAmperage
      unsigned char bListenEVSEStatus; //should we pay attention to stop requests and such yet?
      unsigned char bDoMismatchChecks; //should we be checking for voltage and current mismatches?
      unsigned char vMismatchCount; //count # of consecutive voltage mismatches. Don't trigger until we get enough
      unsigned char cMismatchCount; //same but for current
      unsigned char vCapCount; //# of EVSE voltage capacity checks that have failed in a row.
      unsigned char vOverFault; //over volt fault counter like above.
      unsigned char faultCount; //force faults to count up a bit before we actually fault.
      unsigned int mismatchStart;
    unsigned short int mismatchDelay; //don't start mismatch checks for 10 seconds
    unsigned int stateMilli;
    unsigned short int stateDelay;
    //Teodora deleted const before and value after mismatchDelay and lastCommTimeout
    //unsigned int insertionTime;
    unsigned int lastCommTime;
    unsigned short int lastCommTimeout; //allow up to 1 second of comm fault before getting angry

    //Teodora need access to chademoState
    //CHADEMOSTATE chademoState = LIMBO;
    CHADEMOSTATE stateHolder;
    //TEODORA: i need to set some values so it could not be private
    //EVSE_PARAMS evse_params;
    //EVSE_STATUS evse_status;
    //CARSIDE_STATUS carStatus;

    void sendCANStatus();
    //void sendCANBattSpecs();
    //void sendCANChargingTime();
};

extern CHADEMO chademo;

#endif
