#ifndef __itkSeparableRadiusImageFilter_h
#define __itkSeparableRadiusImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"


namespace itk {

/**
 * \class SeparableRadiusImageFilter
 * \brief A separable rank filter
 * 
 * Medians aren't separable, but if you want a large robust smoother
 * to be relatively quick then it is worthwhile pretending that they
 * are.
 *
 * \author Richard Beare
 */

template<class TInputImage, class TOutputImage, class TFilter>
class ITK_EXPORT SeparableRadiusImageFilter : 
public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SeparableRadiusImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SeparableRadiusImageFilter,
               ImageToImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  
  typedef TOutputImage OutputImageType;
  typedef typename TOutputImage::PixelType OutputPixelType ;

  typedef TFilter FilterType ;
  typedef CastImageFilter< InputImageType, OutputImageType > CastType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

  // itkSetMacro(Radius, RadiusType);
  void SetRadius( const RadiusType );
  itkGetMacro(Radius, RadiusType);

  void GenerateInputRequestedRegion() ;

  virtual void Modified() const;

protected:
  SeparableRadiusImageFilter();
  ~SeparableRadiusImageFilter() {};

  void GenerateData();

  typedef typename itk::Neighborhood<bool, TInputImage::ImageDimension> KernelType;

  KernelType m_kernels[ImageDimension];

  typename FilterType::Pointer m_Filters[ImageDimension];
  
  typename CastType::Pointer m_Cast;

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
