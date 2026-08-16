#ifndef UHARDWARECATALOGPATHS_H
#define UHARDWARECATALOGPATHS_H

#include <QString>

namespace RDK {

class UHardwareCatalogPaths {
public:
    static QString bundledCatalogRelativeRoot();
    static QString catalogRoot();
    static QString catalogIndexPath();
};

} // namespace RDK

#endif
