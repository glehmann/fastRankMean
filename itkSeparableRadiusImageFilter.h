#ifndef __itkSeparableRadiusImageFilter_h
#define __itkSeparableRadiusImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkRankImageFilter.h"

namespace itk {

/**
 * \class SeparableRadiusImageFilter
 * \brief A separable rank filter
 *
 * \author Gaetan Lehmann
 */

template<class TImage, class TFilter>
class ITK_EXPORT SeparableRadiusImageFilter : 
public ImageToImageFilter<TImage, TImage>
{
public:
  /** Standard class typedefs. */
  typedef SeparableRadiusImageFilter Self;
  typedef ImageToImageFilter<TImage,TImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SeparableRadiusImageFilter,
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
  SeparableRadiusImageFilter();
  ~SeparableRadiusImageFilter() {};

  void GenerateData();

  typedef typename itk::Neighborhood<bool, TImage::ImageDimension> KernelType;

  typename FilterType::Pointer m_Filters[ImageDimension];

private:
  SeparableRadiusImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  RadiusType m_Radius;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSeparableRadiusImageFilter.txx"
#endif

#endif
