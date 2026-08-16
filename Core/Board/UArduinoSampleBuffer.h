#ifndef UARDUINOSAMPLEBUFFER_H
#define UARDUINOSAMPLEBUFFER_H

namespace RDK {

template<class T>
class MDMatrix;

struct UArduinoSampleBuffer {
    static void appendRow(MDMatrix<double>& matrix, const double* row, int cols);
    static void trimRows(MDMatrix<double>& matrix, int max_rows);
    static void clear(MDMatrix<double>& matrix);
};

} // namespace RDK

#endif
