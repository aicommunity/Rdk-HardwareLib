#include "UArduinoSampleBuffer.h"

#include "../../../../Rdk/Core/Math/MDMatrix.h"

#include <QtGlobal>

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

void UArduinoSampleBuffer::appendRowBounded(MDMatrix<double>& matrix, const double* row, int cols,
                                            int max_rows)
{
    if (!row || cols <= 0)
        return;

    max_rows = max_rows > 0 ? max_rows : 1;
    const int targetCols = qMax(matrix.GetCols(), cols);
    if (matrix.GetCols() != targetCols)
        matrix.Resize(matrix.GetRows(), targetCols, 0.0);

    int rows = matrix.GetRows();
    if (rows >= max_rows) {
        // Drop a batch to avoid shifting the whole matrix on every new frame.
        const int dropRows = qMax(1, max_rows / 4);
        matrix.DeleteRows(0, qMin(rows, dropRows));
        rows = matrix.GetRows();
    }

    matrix.InsertRows(rows, 1);
    for (int c = 0; c < targetCols; ++c)
        matrix(rows, c) = 0.0;
    for (int c = 0; c < cols; ++c)
        matrix(rows, c) = row[c];
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
