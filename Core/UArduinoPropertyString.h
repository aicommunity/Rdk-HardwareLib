#ifndef UARDUINOPROPERTYSTRING_H
#define UARDUINOPROPERTYSTRING_H

#include <QString>
#include <string>

namespace RDK {

/** Convert between QString and std::string stored in UProperty / MModel. */
namespace UArduinoPropertyString {

QString fromStdProperty(const std::string& value);
std::string toStdProperty(const QString& value);

} // namespace UArduinoPropertyString

} // namespace RDK

#endif
