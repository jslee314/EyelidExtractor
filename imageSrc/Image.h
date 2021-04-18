// Image.h
//
// Author: Robert Crandall
//
// Generic, templated Image class
// Contains pseudo 2d array of type T, and functions
// for memory management

#ifndef IMAGE_H
#define IMAGE_H

#include <assert.h>

template <typename T>
class Image
{
  public:

    // Constructors
	  Image() {
		  m_Data = 0;
		  m_nRows = 0;
		  m_nCols = 0;
	  }
	  Image(unsigned int nr, unsigned int nc) {
		  m_Data = 0;
		  Allocate(nr, nc);
	  }

    // Destructor; calls Free()
    ~Image(){Free();}

    // Free memory allocated by this class

	void Free()
	{
		// Don't delete if pointer is null
		if (0 != m_Data)
		{
			// Delete row data
			for (unsigned int i = 0; i < m_nRows; ++i)
			{
				if (0 != m_Data[i])
				{
					delete[] m_Data[i];
					m_Data[i] = 0;
				}
			}
			// Delete row pointers
			delete[] m_Data;

			m_Data = 0;
		}

		m_nRows = 0;
		m_nCols = 0;
	}

    // Set every pixel in the image to a given value
	void Set(T val)
	{
		for (unsigned int i = 0; i < m_nRows; ++i)
		{
			for (unsigned int j = 0; j < m_nCols; ++j)
			{
				m_Data[i][j] = val;
			}
		}
	}

    // Reallocate memory for a certain size image
	void Allocate(unsigned int nr, unsigned int nc)
	{
		// Make sure memory isn't already allocated; if it is, do nothing
		if (nr != m_nRows || nc != m_nCols)
		{
			// Free memory if there is any allocated
			if (0 != m_Data)
			{
				Free();
			}

			m_nRows = nr;
			m_nCols = nc;

			// Allocate memory for row pointers
			m_Data = new T*[m_nRows];

			// Allocate memory for row data
			for (unsigned int i = 0; i < m_nRows; ++i)
			{
				m_Data[i] = new T[m_nCols];
			}
		}
	}

    // Return a sub-image of current image
	Image<T>* subImage(unsigned int rowStart,
		unsigned int colStart,
		unsigned int rowEnd,
		unsigned int colEnd)
	{
		// Check bounds
		assert(rowStart >= 0 && rowStart < m_nRows
			&& rowEnd >= 0 && rowEnd < m_nRows
			&& colStart >= 0 && colStart < m_nCols
			&& colEnd >= 0 && colEnd < m_nCols);

		Image<T>* sub = new Image<T>(rowEnd - rowStart + 1, colEnd - colStart + 1);
		for (unsigned int i = rowStart; i <= rowEnd; ++i)
		{
			for (unsigned int j = colStart; j <= colEnd; ++j)
			{
				sub->data()[i - rowStart][j - colStart] = m_Data[i][j];
			}
		}

		return sub;
	}


    // Copy data from another image of the same type,
    // resizing if necessary
	void CopyFrom(const Image<T>* imageIn)
	{
		unsigned int nr = imageIn->nRows();
		unsigned int nc = imageIn->nCols();

		// Resize and reallocate memory if necessary
		Allocate(nr, nc);

		// Copy image data
		for (unsigned int i = 0; i < nr; ++i)
		{
			for (unsigned int j = 0; j < nc; ++j)
			{
				m_Data[i][j] = (imageIn->data())[i][j];
			}
		}
	}

    // Copy data from an image of smaller or equal size
    // into this image, starting at a given row/col index
	void CopyFrom(const Image<T>* imageIn,
		unsigned int rowStart,
		unsigned int colStart)
	{
		unsigned int nr = imageIn->nRows();
		unsigned int nc = imageIn->nCols();
		unsigned int rowEnd = rowStart + nr - 1;
		unsigned int colEnd = colStart + nc - 1;

		// Check bounds
		assert(rowStart >= 0 && rowStart < m_nRows
			&& rowEnd >= 0 && rowEnd < m_nRows
			&& colStart >= 0 && colStart < m_nCols
			&& colEnd >= 0 && colStart < m_nCols);

		for (unsigned int i = rowStart; i <= rowEnd; ++i)
		{
			for (unsigned int j = colStart; j <= colEnd; ++j)
			{
				m_Data[i][j] = imageIn->data()[i - rowStart][j - rowStart];
			}
		}
	}


    // Accessors
    T** data() const {return m_Data;}
    unsigned int nRows() const {return m_nRows;}
    unsigned int nCols() const {return m_nCols;}

  private:

    // Image data
    T** m_Data;

    // Image info
    unsigned int m_nRows;
    unsigned int m_nCols;
};
// Need to do this to avoid undefined reference erros for
// templated classes in GNU make... haven't found a better
// way yet
template class Image<double>;
template class Image<unsigned char>;
template class Image<unsigned int>;
#endif