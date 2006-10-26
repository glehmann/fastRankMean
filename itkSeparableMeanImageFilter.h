#ifndef __itkFastApproxRankImageFilter_h
#define __itkFastApproxRankImageFilter_h

#include "itkSeparableImageFilter.h"
#include "itkMovingWindowMeanImageFilter.h"

namespace itk {

/**
 * \class SeparableMeanImageFilter
 * \brief A separable mean filter
 * 
 * \author Gaetan Lehmann
 */

template<class TInputImage, class TOutputImage>
class ITK_EXPORT SeparableMeanImageFilter : 
public SeparableImageFilter<TInputImage, TOutputImage, MovingWindowMeanImageFilter<TOutputImage, TOutputImage, Neighborhood<bool, TInputImage::ImageDimension> > >
{
public:
  /** Standard class typedefs. */
  typedef SeparableMeanImageFilter Self;
  typedef SeparableImageFilter<TInputImage, TOutputImage, MovingWindowMeanImageFilter<TOutputImage, TOutputImage, Neighborhood<bool, TInputImage::ImageDimension> > > Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(SeparableMeanImageFilter,
               SeparableImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::FilterType FilterType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

protected:
  SeparableMeanImageFilter() {};
  ~SeparableMeanImageFilter() {};

private:
  SeparableMeanImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

}

#endif
