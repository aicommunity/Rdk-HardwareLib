#ifndef UDCCONTROLDEMO_H
#define UDCCONTROLDEMO_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoControl.h"


namespace RDK {

class RDK_LIB_TYPE UDcControlDemo: public UNet
{

protected:
    UEPtr<UArduinoControl>UArdContr;

public:

    // - ,         
    UProperty<string, UDcControlDemo, ptPubParameter| ptOutput> Command;

    //  -    
    UProperty<bool, UDcControlDemo, ptPubState> SendCommandFlag;

    //  -  ,   
    UProperty<string, UDcControlDemo, ptPubState> SentCommand;

    //  - 
    UProperty<float, UDcControlDemo, ptPubState> Speed;

    //  - 
    UProperty<float, UDcControlDemo, ptPubState> Acceleration;

    //  -    
    UProperty<bool, UDcControlDemo, ptPubState> GetSpeed;

public:
 UDcControlDemo(void);
 virtual ~UDcControlDemo(void);
 virtual UDcControlDemo* New(void);
 // void SetSpeed(int speed);

  protected:
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

 virtual void AInit(void);
 virtual void AUnInit(void);
};
}

#endif // UDCCONTROLDEMO_H
