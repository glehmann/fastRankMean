// histogram from the moving histogram operations
#ifndef __itkRankHistogram_h
#define __itkRankHistogram_h
#include "itkNumericTraits.h"

namespace itk {

// a simple histogram class hierarchy. One subclass will be maps, the
// other vectors.
// This version is intended for keeping track of arbitary ranks. It is
// based on the code from consolidatedMorphology.
//
// Support for different TCompare hasn't been tested, and shouldn't be
// necessary for the rank filters.
//
#include <sstream>

template <class TInputPixel>
class RankHistogram
{
public:
  RankHistogram() 
  {
    m_Rank = 0.5;
  }
  virtual ~RankHistogram(){}

  virtual RankHistogram *Clone(){return 0;}
  
  virtual void AddPixel(const TInputPixel &p){}

  virtual void RemovePixel(const TInputPixel &p){}
 
  void AddBoundary(){}

  void RemoveBoundary(){}
 
  virtual TInputPixel GetValue(){return 0;}

  void SetRank(float rank)
  {
    m_Rank = rank;
  }

//   virtual std::string PrintHist(){return("");}

protected:
  float m_Rank;
};

template <class TInputPixel, class TCompare>
class RankHistogramMap : public RankHistogram<TInputPixel>
{
private:
  typedef typename std::map< TInputPixel, unsigned long, TCompare > MapType;
  
  MapType m_Map;
  unsigned long m_Below, m_Entries;
  TInputPixel m_RankValue, m_InitVal;
  TCompare m_Compare;
  bool m_Initialized;
  // This iterator will point at the desired rank value
  typename MapType::iterator m_RankIt;

public:
  RankHistogramMap() 
  {
    m_Below = m_Entries = 0;
    // can't set m_RankIt until something has been put in the histogram
    m_Initialized = false;
    if( m_Compare( NumericTraits< TInputPixel >::max(), 
		   NumericTraits< TInputPixel >::NonpositiveMin() ) )
      {
      m_InitVal = NumericTraits< TInputPixel >::NonpositiveMin();
      }
    else
      {
      m_InitVal = NumericTraits< TInputPixel >::max();
      }
    m_RankValue = m_InitVal;
    m_RankIt = m_Map.begin();  // equivalent to setting to the intial value
   }
  ~RankHistogramMap()
  {
  }

  void AddPixel(const TInputPixel &p)
  {
    m_Map[ p ]++;
    ++m_Entries;
    if (!m_Initialized)
      {
      m_Initialized = true;
      m_RankIt = m_Map.begin();
      m_RankValue = p;
      }
    if (m_Compare(p, m_RankValue) || p == m_RankValue)
      {
      ++m_Below;
      }

  }

  void RemovePixel(const TInputPixel &p)
  {
    m_Map[ p ]--; 
    if (m_Compare(p, m_RankValue) || p == m_RankValue)
      {
      --m_Below;
      }
    --m_Entries;
  }
 
  void Initialize()
  {
    m_RankIt = m_Map.begin();
  }

  TInputPixel GetValue()
  {
    unsigned long target = (int)(this->m_Rank * (m_Entries-1)) + 1;
    unsigned long total = m_Below;
    unsigned long ThisBin;
    bool eraseFlag = false;

    // an assert is better than a log message in that case
    assert(m_Initialized);

    if (total < target)
      {
      typename MapType::iterator searchIt = m_RankIt;
      typename MapType::iterator eraseIt;
      
      while (searchIt != m_Map.end())
	{
	// cleaning up the map - probably a better way of organising
	// the loop. Currently makes sure that the search iterator is
	// incremented before deleting
	++searchIt;
	ThisBin = searchIt->second;
	total += ThisBin;
	if (eraseFlag)
	  {
	  m_Map.erase(eraseIt);
	  eraseFlag = false;
	  }
	if (ThisBin <= 0)
	  {
	  eraseFlag = true;
	  eraseIt = searchIt;
	  }
	if (total >= target)
	  break;
	}
      m_RankValue = searchIt->first;
      m_RankIt = searchIt;
      }
    else
      {
      typename MapType::iterator searchIt = m_RankIt;
      typename MapType::iterator eraseIt;

      while(searchIt != m_Map.begin())
	{
	ThisBin = searchIt->second;
	unsigned int tbelow = total - ThisBin;
	if (tbelow < target) // we've overshot
	  break;
	if (eraseFlag)
	  {
	  m_Map.erase(eraseIt);
	  eraseFlag = false;
	  }
	if (ThisBin <= 0)
	  {
	  eraseIt = searchIt;
	  eraseFlag = true;
	  }
	total = tbelow;				
// 	std::cout << searchIt->first << std::endl;

	--searchIt;
	}
      m_RankValue = searchIt->first;
      m_RankIt = searchIt;
      }

    m_Below = total;

    return(m_RankValue);

  }

};

template <class TInputPixel, class TCompare>
class RankHistogramVec : public RankHistogram<TInputPixel>
{
private:
  typedef typename std::vector<unsigned long> VecType;
  
  VecType m_Vec;
  unsigned int m_Size;
  TCompare m_Compare;
  //unsigned int m_CurrentValue;
  //TInputPixel m_CurrentValue;
  TInputPixel m_RankValue;
  TInputPixel m_InitVal;
  int m_Below;
  int m_Entries;

public:
  RankHistogramVec() 
  {
    m_Size = static_cast<unsigned int>( NumericTraits< TInputPixel >::max() - 
					NumericTraits< TInputPixel >::NonpositiveMin() + 1 );
    m_Vec.resize(m_Size, 0 );
    if( m_Compare( NumericTraits< TInputPixel >::max(), 
		   NumericTraits< TInputPixel >::NonpositiveMin() ) )
      {
      m_InitVal = NumericTraits< TInputPixel >::NonpositiveMin();
      }
    else
      {
      m_InitVal = NumericTraits< TInputPixel >::max();
      }
    m_Entries = m_Below = 0;
    m_RankValue = m_InitVal  - NumericTraits< TInputPixel >::NonpositiveMin();
  }

  RankHistogramVec(bool NoInit) 
  {
    m_Size = static_cast<unsigned int>( NumericTraits< TInputPixel >::max() - 
					NumericTraits< TInputPixel >::NonpositiveMin() + 1 );
    if (!NoInit) m_Vec.resize(m_Size, 0 );
    if( m_Compare( NumericTraits< TInputPixel >::max(), 
		   NumericTraits< TInputPixel >::NonpositiveMin() ) )
      {
      m_InitVal = NumericTraits< TInputPixel >::NonpositiveMin();
      }
    else
      {
      m_InitVal = NumericTraits< TInputPixel >::max();
      }
    m_Entries = m_Below = 0;
    m_RankValue = m_InitVal  - NumericTraits< TInputPixel >::NonpositiveMin();
  }


  ~RankHistogramVec()
  {
  }

  TInputPixel GetValue()
  {
    unsigned long target = (int)(this->m_Rank * (m_Entries-1)) + 1;
    unsigned long total = m_Below;
    long unsigned int pos = (long unsigned int)(m_RankValue - NumericTraits< TInputPixel >::NonpositiveMin()); 

    if (total < target)
      {
      while (pos < m_Size)
	{
	++pos;
	total += m_Vec[pos];
	if (total >= target)
	  break;
	}
      }
    else
      {
      while(pos > 0)
	{
	unsigned int tbelow = total - m_Vec[pos];
	if (tbelow < target) // we've overshot
	  break;
	total = tbelow;
	--pos;
	}
      }

    m_RankValue = (TInputPixel)(pos + NumericTraits< TInputPixel >::NonpositiveMin());
    m_Below = total;
    return(m_RankValue);
  }

  void AddPixel(const TInputPixel &p)
  {
    long unsigned int idx = (long unsigned int)(p - NumericTraits< TInputPixel >::NonpositiveMin());
    m_Vec[ idx  ]++; 
    if (m_Compare(p, m_RankValue) || p == m_RankValue)
      {
      ++m_Below;
      }
    ++m_Entries;
  }

  void RemovePixel(const TInputPixel &p)
  {
    assert(p - NumericTraits< TInputPixel >::NonpositiveMin() >= 0);
    assert(p - NumericTraits< TInputPixel >::NonpositiveMin() < (int)m_Vec.size());
    assert(m_Entries >= 1);
    m_Vec[ (long unsigned int)(p - NumericTraits< TInputPixel >::NonpositiveMin())  ]--; 
    --m_Entries;

    if (m_Compare(p, m_RankValue) || p == m_RankValue)
      {
      --m_Below;
      }
  }
 
};

} // end namespace itk
#endif
