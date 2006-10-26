#ifndef __itkSeparableImageFilter_h
#define __itkSeparableImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkRankImageFilter.h"

namespace itk {

/**
 * \class SeparableImageFilter
 * \brief A separable rank filter
 * 
 * Medians aren't separable, but if you want a large robust smoother
 * to be relatively quick then it is worthwhile pretending that they
 * are.
 *
 * \author Richard Beare
 */

template<class TImage, class TFilter>
class ITK_EXPORT SeparableImageFilter : 
public ImageToImageFilter<TImage, TImage>
{
public:
  /** Standard class typedefs. */
  typedef SeparableImageFilter Self;
  typedef ImageToImageFilter<TImage,TImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SeparableImageFilter,
               ImageToImageFilter);
 
  /** Image related typedefs. */
  typedef TImage InputImageType;
  typedef typename TImage::RegionType RegionType ;
  typedef typename TImage::SizeType SizeType ;
  typedef typename TImage::IndexType IndexType ;
  typedef typename TImage::PixelType PixelType ;
  typedef typename TImage::OffsetType OffsetType ;

  typedef TFilter FilterType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TImage::SizeType RadiusType ;

  // itkSetMacro(Radius, RadiusType);
  void SetRadius( const RadiusType );
  itkGetMacro(Radius, RadiusType);

  void GenerateInputRequestedRegion() ;

  virtual void Modified() const;

protected:
  SeparableImageFilter();
  ~SeparableImageFilter() {};

  void GenerateData();

  typedef typename itk::Neighborhood<bool, TImage::ImageDimension> KernelType;

  KernelType m_kernels[ImageDimension];

  typename FilterType::Pointer m_Filters[ImageDimension];

private:
  SeparableImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  RadiusType m_Radius;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSeparableImageFilter.txx"
#endif

#endif
