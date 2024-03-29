#ifndef __itkKernelImageFilter_h
#define __itkKernelImageFilter_h

#include "itkBoxImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkNeighborhood.h"


namespace itk {

/**
 * \class KernelImageFilter
 * \brief A base class for all the filters working on an arbitrary shaped neighborhood
 *
 * This filter provides the code to store the radius information about the
 * neighborhood used in the subclasses.
 * It also conveniently reimplement the GenerateInputRequestedRegion() so
 * that region is well defined for the porvided radius.
 *
 * \author Gaetan Lehmann
 */

template<class TInputImage, class TOutputImage, class TKernel/*=Neighborhood<bool, TInputImage::ImageDimension>*/ >
class ITK_EXPORT KernelImageFilter : 
public BoxImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef KernelImageFilter Self;
  typedef BoxImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(KernelImageFilter,
               BoxImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::OffsetType OffsetType ;

  typedef typename TInputImage::PixelType InputPixelType ;
  
  typedef TOutputImage OutputImageType;
  typedef typename TOutputImage::PixelType OutputPixelType ;

  typedef TKernel KernelType;

  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

  /** Set kernel (structuring element). */
  virtual void SetKernel( const KernelType& kernel );
  virtual void SetRadius( const unsigned long & radius )
    {
    // needed because of the overloading of the method
    Superclass::SetRadius( radius );
    }


  /** Get/Set the kernel (structuring element). */
  itkGetConstReferenceMacro(Kernel, KernelType);
  
  virtual void SetRadius( const RadiusType & radius );

protected:
  KernelImageFilter();
  ~KernelImageFilter() {};

  void PrintSelf(std::ostream& os, Indent indent) const;

  /** kernel or structuring element to use. */
  KernelType m_Kernel ;

private:
  KernelImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkKernelImageFilter.txx"
#endif

#endif
