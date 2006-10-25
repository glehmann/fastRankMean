/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMovingHistogramRankImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMovingHistogramRankImageFilter_txx
#define __itkMovingHistogramRankImageFilter_txx

#include "itkMovingHistogramRankImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"
#include "itkNumericTraits.h"


#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"

#include <iomanip>
#include <sstream>

namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
MovingHistogramRankImageFilter<TInputImage, TOutputImage, TKernel>
::MovingHistogramRankImageFilter()
{
  m_Rank = 0.5;
}


template<class TInputImage, class TOutputImage, class TKernel>
typename MovingHistogramRankImageFilter<TInputImage, TOutputImage, TKernel>::HistogramType *
MovingHistogramRankImageFilter<TInputImage, TOutputImage, TKernel>
::NewHistogram()
{
  HistogramType * hist;
  if (useVectorBasedHistogram())
    {
    hist = new VHistogram();
    }
  else
    {
    hist = new MHistogram();
    }
  hist->SetRank( this->GetRank() );
  return hist;
}


template<class TInputImage, class TOutputImage, class TKernel>
void
MovingHistogramRankImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Rank: " << static_cast<typename NumericTraits< float >::PrintType>( m_Rank ) << std::endl;
}

}// end namespace itk
#endif
