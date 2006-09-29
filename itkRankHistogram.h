// histogram from the moving histogram operations
#ifndef __itkRankHistogram_h
#define __itkRankHistogram_h
#include "itkNumericTraits.h"

namespace itk {

// a simple histogram class hierarchy. One subclass will be maps, the
// other vectors
template <class TInputPixel>
class RankHistogram
{
public:
  RankHistogram() {}
  ~RankHistogram(){}

  virtual RankHistogram *Clone(){}
  
  virtual void Reset(){}
  
  virtual void AddBoundary(){}

  virtual void RemoveBoundary(){}
  
  virtual void AddPixel(const TInputPixel &p){}

  virtual void RemovePixel(const TInputPixel &p){}
 
  virtual TInputPixel GetValue(){}

  virtual TInputPixel GetRankValue(float rank){}

  void SetBoundary( const TInputPixel & val )
  {
    m_Boundary = val; 
  }
protected:
  TInputPixel  m_Boundary;

};

template <class TInputPixel, class TCompare>
class RankHistogramMap : public RankHistogram<TInputPixel>
{
private:
  typedef typename std::map< TInputPixel, unsigned long, TCompare > MapType;
  
  MapType m_Map;

public:
  RankHistogramMap() 
  {
  }
  ~RankHistogramMap(){}

  RankHistogramMap *Clone()
  {
    RankHistogramMap *result = new RankHistogramMap();
    result->m_Map = this->m_Map;
    return(result);
  }
  void Reset()
  {
    m_Map.clear();
  }
  
  void AddBoundary()
  {
    m_Map[ this->m_Boundary ]++;
  }

  void RemoveBoundary()
  {
    m_Map[ this->m_Boundary ]--; 
  }
  
  void AddPixel(const TInputPixel &p)
  {
    m_Map[ p ]++; 
  }

  void RemovePixel(const TInputPixel &p)
  {
    m_Map[ p ]--; 
  }
 
  TInputPixel GetValue()
  {    // clean the map
    typename MapType::iterator mapIt = m_Map.begin();
    while( mapIt != m_Map.end() )
      {
      if( mapIt->second <= 0 )
        {
        // this value must be removed from the histogram
        // The value must be stored and the iterator updated before removing the value
        // or the iterator is invalidated.
        TInputPixel toErase = mapIt->first;
        mapIt++;
        m_Map.erase( toErase );
        }
      else
	{
        mapIt++;
        // don't remove all the zero value found, just remove the one before the current maximum value
        // the histogram may become quite big on real type image, but it's an important increase of performances
        break;
        }
      }
    
    // and return the value
    return m_Map.begin()->first;
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
  TInputPixel m_CurrentValue;
  TInputPixel m_InitVal;
  int m_Direction;
  int m_Entries;
//  int m_Additions, m_Removals;
public:
  RankHistogramVec() 
  {
    m_Size = static_cast<unsigned int>( NumericTraits< TInputPixel >::max() - 
					NumericTraits< TInputPixel >::NonpositiveMin() + 1 );
    m_Vec.resize(m_Size, 0 );
    if( m_Compare( NumericTraits< TInputPixel >::max(), 
		   NumericTraits< TInputPixel >::NonpositiveMin() ) )
      {
      m_CurrentValue = m_InitVal = NumericTraits< TInputPixel >::NonpositiveMin();
      m_Direction = -1;
      }
    else
      {
      m_CurrentValue = m_InitVal = NumericTraits< TInputPixel >::max();
      m_Direction = 1;
      }
    m_Entries = 0;
//    m_Additions=m_Removals=0;
  }
  ~RankHistogramVec(){}

  RankHistogramVec *Clone()
  {
    RankHistogramVec *result = new RankHistogramVec();
    result->m_Vec = this->m_Vec;
    result->m_Size = this->m_Size;
    result->m_CurrentValue = this->m_CurrentValue;
    result->m_InitVal = this->m_InitVal;
    result->m_Direction = this->m_Direction;
    result->m_Entries = this->m_Entries;
    return(result);
  }

  TInputPixel GetRankValue(float rank)
  {
    // simple minded approach - computes the rank from scratch every
    // time
    unsigned long target = (int)(rank * m_Entries);
    unsigned long total = 0;
    TInputPixel pos = m_CurrentValue - NumericTraits< TInputPixel >::NonpositiveMin();
//    std::cout << m_Entries << " " << (int)m_CurrentValue;
    for (;;)
      {
//      std::cout << "[" << (int)pos << " " << m_Vec[pos] << "]";
      total += m_Vec[pos];
      if (total > target)
	{
//	std::cout << std::endl;
	return(pos);
	}
      ++pos;
      }
  }

  void Reset(){
    std::fill(&(m_Vec[0]), &(m_Vec[m_Size-1]),0);
    m_CurrentValue = m_InitVal;
    m_Entries = 0;
  }
  
  void AddBoundary()
  {
    AddPixel(this->m_Boundary);
    ++m_Entries;
  }

  void RemoveBoundary(){
    RemovePixel(this->m_Boundary);
    --m_Entries;
  }
  
  void AddPixel(const TInputPixel &p)
  {
    m_Vec[ (long unsigned int)(p - NumericTraits< TInputPixel >::NonpositiveMin())  ]++; 
    if (m_Compare(p, m_CurrentValue))
      {
      m_CurrentValue = p;
      }
    ++m_Entries;
  }

  void RemovePixel(const TInputPixel &p)
  {
    assert(p - NumericTraits< TInputPixel >::NonpositiveMin() >= 0);
    assert(p - NumericTraits< TInputPixel >::NonpositiveMin() < m_Vec.size());
    assert(m_Entries >= 1);
    m_Vec[ (long unsigned int)(p - NumericTraits< TInputPixel >::NonpositiveMin())  ]--; 
    --m_Entries;
    assert(static_cast<int>(m_CurrentValue -                                                                                                                      
			    NumericTraits< TInputPixel >::NonpositiveMin() ) >= 0);
    assert(static_cast<int>(m_CurrentValue -                                                                                                                      
			    NumericTraits< TInputPixel >::NonpositiveMin() ) < m_Vec.size());
    if (m_Entries > 0)
      {
      while( m_Vec[static_cast<int>(m_CurrentValue - 
				    NumericTraits< TInputPixel >::NonpositiveMin() )] == 0 )
	{
	m_CurrentValue += m_Direction;
	assert(static_cast<int>(m_CurrentValue -                                                                                                                      
				NumericTraits< TInputPixel >::NonpositiveMin() ) >= 0);
	assert(static_cast<int>(m_CurrentValue -                                                                                                                      
				NumericTraits< TInputPixel >::NonpositiveMin() ) < m_Vec.size());
	}
      }
  }
 
  TInputPixel GetValue()
  { 
    assert(m_Entries > 0);
    return(m_CurrentValue);
  }

};

} // end namespace itk
#endif
