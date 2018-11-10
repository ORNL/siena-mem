#ifndef AR_CACHE_H
#define AR_CACHE_H


#include <cstring>
#include <iomanip>
#include <cstdint>
#include <sstream>

#ifdef PIN
#include "rthms_config.h"
#else
#include "ar_typedef.h"
#endif

#ifdef DRAMSIM
namespace ARCache{
  typedef uint64_t UINT64;
}
typedef ARCache::UINT64 CACHE_STATS; // type of cache hit/miss counters
#else
typedef uint64_t UINT64;
typedef UINT64 CACHE_STATS;
#endif

namespace CACHE_PROTOCOL
{
    typedef enum WRITE_POLICY{
      WRITE_BACK,
      WRITE_THROUGH
    }WRITE_POLICY;

    typedef enum WRITE_MISS_POLICY{
      WRITE_ALLOCATE,
      NO_WRITE_ALLOCATE
    }WRITE_MISS_POLICY;
}

typedef void     VOID;
typedef uint64_t UINT32;
typedef uint64_t UINT64;
typedef int64_t  INT32;
typedef int64_t  INT64;
typedef uint64_t ADDRINT;
typedef char     CHAR;
typedef double   FLT64;
typedef float    FLT32;

using namespace std;
using namespace CACHE_PROTOCOL;

#ifndef PIN
inline string StringFlt(FLT64 val,UINT32 precision, UINT32 width){
  stringstream ss;
  ss << std::right<< std::fixed<<setw(width)<<setprecision(precision)<<val;
  return ss.str();
}
#define ASSERTX(expr) ( (expr) || printf("%s %s %i \n",#expr,__FILE__,__LINE__) )
inline string ljstr(const string& s, UINT32 width, CHAR padding = ' ')
{
  string  ostr(width,padding);
  ostr.replace(0,s.length(),s);
  return ostr;
}
inline string fltstr(FLT64 val, UINT32 prec=0,UINT32 width=0 ){
    return StringFlt(val,prec,width);
}
#endif

#define KILO 1024
#define MEGA (KILO*KILO)
#define GIGA (KILO*MEGA)


/*!
 *  @brief Checks if n is a power of 2.
 *  @returns true if n is power of 2
 */
static inline bool IsPower2(UINT32 n)
{
    return ((n & (n - 1)) == 0);
}

/*!
 *  @brief Computes floor(log2(n))
 *  Works by finding position of MSB set.
 *  @returns -1 if n == 0.
 */
static inline INT32 FloorLog2(UINT32 n)
{
    INT32 p = 0;
    
    if (n == 0) return -1;
    
    if (n & 0xffff0000) { p += 16; n >>= 16; }
    if (n & 0x0000ff00)	{ p +=  8; n >>=  8; }
    if (n & 0x000000f0) { p +=  4; n >>=  4; }
    if (n & 0x0000000c) { p +=  2; n >>=  2; }
    if (n & 0x00000002) { p +=  1; }
    
    return p;
}

/*!
 *  @brief Computes floor(log2(n))
 *  Works by finding position of MSB set.
 *  @returns -1 if n == 0.
 */
static inline INT32 CeilLog2(UINT32 n)
{
    return FloorLog2(n - 1) + 1;
}


/*!
 * Everything related to cache sets
 */
namespace CACHE_SET
{
    typedef struct CACHEBLOCK{
      int       valid;
      int       dirty;
      ADDRINT   tag;
      ADDRINT   addr;
      //int     memory;//for multi-memory
      uint64_t   timestamp; //for LRU
    }CACHEBLOCK;
    
    typedef enum CACHELOOKUP{
      HIT_VALID,
      HIT_INVALID,
      MISS
    }CACHELOOKUP;
    
    template <WRITE_POLICY write_policy,WRITE_MISS_POLICY write_miss_policy>
    class LRU
    {
    private:
        UINT32      _associativity;
        CACHEBLOCK  *_blocks;
        uint64_t     _timestamp;
        WRITE_POLICY _write_policy;
        WRITE_MISS_POLICY _write_miss_policy;

    public:
        LRU(UINT32 associativity)
        :_associativity(associativity),
        _write_policy(write_policy),
        _write_miss_policy(write_miss_policy)
        {
            _blocks = new CACHEBLOCK[associativity];
             for (UINT32 index = 0; index<_associativity; index++){
                  _blocks[index].valid = 0;
                  _blocks[index].dirty = 0;
                  _blocks[index].tag = (ADDRINT)0;
             }
             _timestamp=0;
        }

        UINT32 GetAssociativity() { return _associativity; }
	    
        CACHELOOKUP Find_and_Update(ADDRINT tag, bool isWrite)
        {

#ifdef DEBUG_CACHE
            for (UINT32 index = 0; index <_associativity; index++)
                DEBUG("LRU::block[%i].tag=%lu,valid=%i,dirty=%i,addr=%lx,timestamp=%lu",
                      index,_blocks[index].tag,_blocks[index].valid,
                      _blocks[index].dirty,_blocks[index].addr,_blocks[index].timestamp);
#endif
            _timestamp ++;
	  
            INT32 found_index = -1;
            for (UINT32 index = 0;index<_associativity; index++)
                if(_blocks[index].tag==tag) found_index=index;
	  
            if(found_index>-1){
                _blocks[found_index].timestamp = _timestamp;
                if(isWrite){
                    if(_write_policy==WRITE_BACK ){
                        _blocks[found_index].valid = 1;
                        _blocks[found_index].dirty = 1;
                    }else{ //writethrough always sync with main memory
                        _blocks[found_index].valid = 1;
                        _blocks[found_index].dirty = 0;
                    }
                    return HIT_VALID;
                }else{//read needs to check valid bit
                    return (_blocks[found_index].valid==1 ? HIT_VALID : HIT_INVALID);
                }
            }
	  
            return MISS;
        }

        CACHEBLOCK Evict_and_Update(ADDRINT tag, ADDRINT new_addr, bool isWrite)
        {
            //Prioirty: invalid > oldest
            UINT32  evict_index = 0;
            UINT32 oldest_index = 0;
            bool found = false;
            UINT32 min_time  = _blocks[0].timestamp;
            for(UINT32 index = 0;index<_associativity; index++)
            {
                if(_blocks[index].valid == 0){
                    evict_index = index;
                    found = true;
                    break;
                }
                if( _blocks[index].timestamp<min_time){
                    min_time = _blocks[index].timestamp;
                    oldest_index = index;
                }
            }
            if(!found) evict_index=oldest_index;
            
            //backup the evicted block first
            CACHEBLOCK evict_block = _blocks[evict_index];

            //update cache block
            _blocks[evict_index].valid= 1;
            _blocks[evict_index].tag  = tag;
            _blocks[evict_index].dirty= (isWrite && _write_policy==WRITE_BACK) ?1 :0 ;
            _blocks[evict_index].addr   = new_addr;
            _blocks[evict_index].timestamp = _timestamp;

#ifdef DEBUG_CACHE
            for (UINT32 index = 0; index <_associativity; index++)
                DEBUG("Evict_and_Update block[%i]: tag=%lu, valid=%i,dirty=%i,addr=0x%lx,timestamp=%lu",
                      index, _blocks[index].tag,_blocks[index].valid,_blocks[index].dirty,
                      _blocks[index].addr,_blocks[index].timestamp);
#endif
      
            return evict_block;
        }

        std::vector<CACHEBLOCK> GetDirtyCacheline(){
            std::vector<CACHEBLOCK> addresses;
            for(INT32 index = 0; index<_associativity; index++)
                if(_blocks[index].valid == 1 && _blocks[index].dirty == 1)
                    addresses.push_back(_blocks[index]);
            return addresses;
        }
 };
   
} // namespace CACHE_SET



class CACHE_BASE
{
public:
    typedef enum
    {
        ACCESS_TYPE_LOAD,
        ACCESS_TYPE_STORE,
        ACCESS_TYPE_NUM
    } ACCESS_TYPE;
    
    typedef enum
    {
        CACHE_TYPE_ICACHE,
        CACHE_TYPE_DCACHE,
        CACHE_TYPE_NUM
    } CACHE_TYPE;
    
    // input params
    const std::string _name;
    const UINT32 _cacheSize;
    const UINT32 _lineSize;
    const UINT32 _associativity;
    
    // computed params
    const UINT32 _lineShift;
    const UINT32 _num_sets;
    const UINT32 _setIndexMask;
    
protected:
    static const UINT32 HIT_MISS_NUM = 2;
    CACHE_STATS _access[ACCESS_TYPE_NUM][HIT_MISS_NUM];


private:
    CACHE_STATS SumAccess(bool hit) const
    {
        CACHE_STATS sum = 0;        
        for (UINT32 accessType = 0; accessType < ACCESS_TYPE_NUM; accessType++)
        {
            sum += _access[accessType][hit];
        }
        
        return sum;
    }
        
public:
    CACHE_BASE(std::string name, UINT32 cacheSize, UINT32 lineSize, UINT32 associativity);
    UINT32 CacheSize() const { return _cacheSize; }
    UINT32 LineSize() const { return _lineSize; }
    UINT32 Associativity() const { return _associativity; }
    CACHE_STATS Hits(ACCESS_TYPE accessType) const { return _access[accessType][true];}
    CACHE_STATS Misses(ACCESS_TYPE accessType) const { return _access[accessType][false];}
    CACHE_STATS Accesses(ACCESS_TYPE accessType) const { return Hits(accessType) + Misses(accessType);}
    CACHE_STATS Hits() const { return SumAccess(true);}
    CACHE_STATS Misses() const { return SumAccess(false);}
    CACHE_STATS Accesses() const { return Hits() + Misses();}
    
    VOID SplitAddress(const ADDRINT addr, ADDRINT & tag, UINT32 & setIndex) const
    {
        tag = addr >> _lineShift;
        setIndex = tag & _setIndexMask;
    }
    
    VOID SplitAddress(const ADDRINT addr, ADDRINT & tag, UINT32 & setIndex, UINT32 & lineIndex) const
    {
        const UINT32 lineMask = _lineSize - 1;
        lineIndex = addr & lineMask;
        SplitAddress(addr, tag, setIndex);
    }
    
    string StatsLong(string prefix = "", CACHE_TYPE = CACHE_TYPE_DCACHE) const;
    void print()const;
};

template <class SET,WRITE_POLICY write_policy, WRITE_MISS_POLICY write_miss_policy>
class CACHE : public CACHE_BASE
{
 private:
    std::vector<SET> _sets;
    const bool _isWriteThrough;
    const bool _isWriteAllocate;

 public:
    CACHE(std::string name,UINT32 cacheSize, UINT32 lineSize, UINT32 associativity)
    :CACHE_BASE(name, cacheSize, lineSize, associativity),
    _isWriteThrough(write_policy==WRITE_POLICY::WRITE_THROUGH),
    _isWriteAllocate(write_miss_policy==WRITE_MISS_POLICY::WRITE_ALLOCATE)
    {
      //_sets = new SET[_num_sets];
      for(size_t i=0;i<_num_sets;i++)
          _sets.push_back(SET(associativity));
    }

    ~CACHE(){}
    
    bool get_isWriteThrough(){ return _isWriteThrough;}
    bool get_isWriteAllocate(){return _isWriteAllocate;}

    CACHE_SET::CACHEBLOCK lookup_evict_update(ADDRINT addr,ACCESS_TYPE accessType)
    {
      ADDRINT tag      = addr >> _lineShift;
      UINT32 setIndex  = tag & _setIndexMask;
      SET &set         = _sets[setIndex];
      bool isWrite     = (accessType==ACCESS_TYPE_STORE);
      bool isRead      = !isWrite;

      //Step 1: Hit or Miss
      CACHE_SET::CACHELOOKUP lookup = set.Find_and_Update(tag,isWrite);
      bool hit = (lookup==CACHE_SET::HIT_VALID || lookup==CACHE_SET::HIT_INVALID);
      _access[accessType][hit]++; //for statistics
      
#ifdef DEBUG_CACHE
      DEBUG("%s::Find_and_Update: %s : addr=%lx, setIndex=%i, tag=%lu\n",
            _name.c_str(),(hit ?"hit" : "miss"), addr, setIndex, tag);
#endif

      //Step 2: evict if read miss or write miss with write_allocate policy
      CACHE_SET::CACHEBLOCK evict_block;
      if(lookup==CACHE_SET::MISS){

          evict_block.dirty=0; //init dirty bit
          if(isRead || _isWriteAllocate)
              evict_block = set.Evict_and_Update(tag,addr,isWrite);

          evict_block.tag = 0; // 0: miss; 1: hit
      }else{
          evict_block.tag = 1; // 0: miss; 1: hit
          evict_block.valid = (lookup==CACHE_SET::HIT_VALID ? 1 : 0);
      }

#ifdef DEBUG_CACHE
      DEBUG("%s::Evict Block: tag%lu, valid%i, dirty%i, addr=%lu\n",
            _name.c_str(),evict_block.tag,evict_block.valid,evict_block.dirty,evict_block.addr);
#endif

    return evict_block;
 }

    void upd_cache_stats(ACCESS_TYPE accessType, bool hit){
        _access[accessType][hit]++;
    }
    std::vector<CACHE_SET::CACHEBLOCK> flush_ditry_cacheline(){
   
        std::vector<CACHE_SET::CACHEBLOCK> addresses;
        for (UINT32 i = 0; i < _num_sets; i++)
        {
            std::vector<CACHE_SET::CACHEBLOCK> tmp = _sets[i].GetDirtyCacheline();
            addresses.insert(addresses.end(), tmp.begin(), tmp.end());
        }
        return addresses;
    }
};


typedef CACHE_SET::LRU<WRITE_BACK,WRITE_ALLOCATE> SET;
typedef CACHE<SET,WRITE_BACK,WRITE_ALLOCATE> CACHE_T;



#endif
