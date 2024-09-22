#ifndef UHardwareLibrary_H
#define UHardwareLibrary_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace RDK {

class RDK_LIB_TYPE UHardwareLibrary: public ULibrary
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UHardwareLibrary(void);
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------
};

extern RDK_LIB_TYPE UHardwareLibrary HardwareLibrary;

}
#endif

