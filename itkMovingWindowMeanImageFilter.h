/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingWindowMeanImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingWindowMeanImageFilter_h
#define __itkMovingWindowMeanImageFilter_h

#include "itkMovingHistogramImageFilter.h"

namespace itk {

namespace Function {
template <class TInputPixel>
class MeanHistogram
{
public:
  MeanHistogram()
    {
    sum = 0;
    count = 0;
    }
  ~MeanHistogram(){}

  inline void AddBoundary() {}

  inline void RemoveBoundary() {}

  inline void AddPixel( const TInputPixel &p )
    {
    sum += p;
    count++;
    }

  inline void RemovePixel( const TInputPixel &p )
    {
    sum -= p;
    count--;
    assert( count >= 0 );
    }

  inline double GetValue()
    {
    return sum / static_cast< double >( count );
    }

  double sum;
  unsigned long count;

};
} // end namespace Function



/**
 * \class MovingWindowMeanImageFilter
 * \brief Mean filter
 * Linear filter where each output pixel is the mean of pixels in an
 * input neighborhood.
 *
 * The boundary conditions of this filter are different to
 * itkMeanImageFilter. In this filter the neighborhood is cropped at
 * the border and therefore becomes smaller.
 * 
 * This filter employs a recursive implementation based on the sliding
 * window code from consolidatedMorphology, and is therefore usually a
 * lot faster than the direct implementation.
 */

template<class TInputImage, class TOutputImage, class TKernel >
class ITK_EXPORT MovingWindowMeanImageFilter : 
    public MovingHistogramImageFilter<TInputImage, TOutputImage, TKernel, typename  Function::MeanHistogram< typename TInputImage::PixelType > >
{
public:
  /** Standard class typedefs. */
  typedef MovingWindowMeanImageFilter Self;
  typedef MovingHistogramImageFilter<TInputImage,TOutputImage, TKernel, typename  Function::MeanHistogram< typename TInputImage::PixelType > >  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MovingWindowMeanImageFilter, 
               MovingHistogramImageFilter);
  
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;
  typedef typename TOutputImage::PixelType OutputPixelType ;
  typedef typename TInputImage::PixelType InputPixelType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      
  /** Kernel typedef. */
  typedef TKernel KernelType;
  
  /** Kernel (structuring element) iterator. */
  typedef typename KernelType::ConstIterator KernelIteratorType ;
  
  /** n-dimensional Kernel radius. */
  typedef typename KernelType::SizeType RadiusType ;

protected:
  MovingWindowMeanImageFilter() {};
  ~MovingWindowMeanImageFilter() {};

private:
  MovingWindowMeanImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

} ; // end of class

} // end namespace itk
  
#endif


