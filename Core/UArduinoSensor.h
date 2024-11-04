#ifndef UARDUINOSENSOR_H
#define UARDUINOSENSOR_H

#include <QObject>
#include <QString>

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "UArduinoChooseFirm.h"

namespace RDK {

class RDK_LIB_TYPE UArduinoSensor: public UNet
{

protected:
QVector<QVector<double>> Data;

protected:
// void RecieveData(double data);

public:
UArduinoSensor(void);
virtual ~UArduinoSensor(void);



// private:
//     void addToSourceMatrix(const QString &data);

};
}

#endif // UARDUINOSENSOR_H
