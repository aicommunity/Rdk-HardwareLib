#include "UArduinoSampleBuffer.h"

#include "../../../Rdk/Core/Math/MDMatrix.h"

namespace RDK {

void UArduinoSampleBuffer::appendRow(MDMatrix<double>& matrix, const double* row, int cols)
{
    if (!row || cols <= 0)
        return;
    const int r = matrix.GetRows();
    matrix.InsertRows(r, 1);
    for (int c = 0; c < cols; ++c)
        matrix(r, c) = row[c];
}

void UArduinoSampleBuffer::trimRows(MDMatrix<double>& matrix, int max_rows)
{
    if (max_rows <= 0) {
        matrix.Resize(0, matrix.GetCols());
        return;
    }
    while (matrix.GetRows() > max_rows) {
        matrix.DeleteRows(0, 1);
    }
}

void UArduinoSampleBuffer::clear(MDMatrix<double>& matrix)
{
    matrix.Resize(0, matrix.GetCols() > 0 ? matrix.GetCols() : 4);
}

} // namespace RDK
