#ifndef UArduinoControl_H
#define UArduinoControl_H

#include <QObject>
#include <QVector>
#include <string>
#include <QTimer>
#include <cmath>
#include <QtDebug>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoConnect.h"


namespace RDK {

class  UArduinoConnect;

class RDK_LIB_TYPE UArduinoControl: public UNet
{
protected:
    UEPtr<UArduinoConnect>UArdConn;
    // UArduinoConnect *UArdConn;

public: //   

    // -    
    UProperty<double, UArduinoControl, ptPubParameter> LowerSensorLimit;

    // -    
    UProperty<double, UArduinoControl, ptPubParameter> UpperSensorLimit;

    // -    
    UProperty<string, UArduinoControl, ptPubParameter> PortToConnect;

    // - ,   
    UProperty<string, UArduinoControl, ptPubParameter | ptInput> Command;

    // -    
    UProperty<int, UArduinoControl, ptPubParameter> MatrixCols;

    //  -    
    UProperty<bool, UArduinoControl, ptPubState> SendCommandFlag;

    //  -    
    // UProperty<bool, UArduinoControl, ptPubState> SendInputCommandFlag;

    //  -  ,   
    UProperty<string, UArduinoControl, ptPubState> SentCommand;

    // ,   UDcControlDemo
    UProperty<string, UArduinoControl, ptPubState | ptInput> InputCommand;

    //  -      
    UProperty<bool, UArduinoControl, ptPubState> GetDataFromBuffers;

    //  
    UProperty<MDMatrix<double>, UArduinoControl, ptPubState> DoubleMatrixReadings;

    //  -     
    UProperty<bool, UArduinoControl, ptPubState> GetPinsInfo;

    //  -     
    UProperty<bool, UArduinoControl, ptPubState> ShowDebug;

    int CurrentRow;
    bool portchanged;

    QString LastCom;

    QVector<double> SpeedValues;

    // QMutex arduinoMutex;

public:
    UArduinoControl(void);
    virtual ~UArduinoControl(void);
    void SendCommand(string command);
    void PutDataToMatrix();

protected:
    void ResetPortChanged();

public:
    //         
    virtual UArduinoControl* New(void);

    // --------------------------
    //    
    // --------------------------
protected:
    bool SetPortToConnect(const string& value);


    ///        
    virtual bool ADefault(void);

    ///     
    ///   
    ///    Reset()   Ready  true
    ///    
    virtual bool ABuild(void);

    ///   .
    virtual bool AReset(void);

    ///    
    virtual bool ACalculate(void);
    // --------------------------

protected:
    virtual void AInit(void);
    virtual void AUnInit(void);

};
}

#endif // UArduinoControl_H
