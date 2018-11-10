#ifdef PIN
#include "pin.H"
#include "rthms_cache.h"
#else
#include "ar_cache.h"
#endif

CACHE_BASE::CACHE_BASE(std::string name, UINT32 cacheSize, UINT32 lineSize, UINT32 associativity)
:
_name(name),
_cacheSize(cacheSize),
_lineSize(lineSize),
_associativity(associativity),
_lineShift(FloorLog2(lineSize)),
_num_sets(cacheSize/(associativity*lineSize)),
_setIndexMask(_num_sets-1)
{
    //ASSERTX(IsPower2(_lineSize));
    //ASSERTX(IsPower2(_num_sets));
    assert(IsPower2(_lineSize));
    assert(IsPower2(_num_sets));

    //init stats
    for (UINT32 accessType = 0; accessType < ACCESS_TYPE_NUM; accessType++)
    {
        _access[accessType][false] = 0;
        _access[accessType][true]  = 0;
    }

}


void CACHE_BASE::print()const
{
    printf("\tassociativity=%lu, capacity=%lu, cacheline=%lu, num_sets=%lu, lineShift=%lu, setIndexMask=%lu\n",
          _associativity,_cacheSize,_lineSize,_num_sets,_lineShift,_setIndexMask);
}


#ifdef DRAMSIM
static string mydecstr(ARCache::UINT64 v, UINT32 w)
#else
static string mydecstr(UINT64 v, UINT32 w)
#endif
{
    ostringstream o;
    o.width(w);
    o << v;
    string str(o.str());
    return str;
}

string CACHE_BASE::StatsLong(string prefix, CACHE_TYPE cache_type) const
{
    const UINT32 headerWidth = 19;
    const UINT32 numberWidth = 12;

    string out;
    
    out += prefix + _name + ":" + "\n";

    if (cache_type != CACHE_TYPE_ICACHE) {
       for (UINT32 i = 0; i < ACCESS_TYPE_NUM; i++)
       {
           const ACCESS_TYPE accessType = ACCESS_TYPE(i);

           std::string type(accessType == ACCESS_TYPE_LOAD ? "Load" : "Store");

           out += prefix + ljstr(type + "-Hits:      ", headerWidth)
                  + mydecstr(Hits(accessType), numberWidth)  +
                  "  " +fltstr(100.0 * Hits(accessType) / Accesses(accessType), 2, 6) + "%\n";

           out += prefix + ljstr(type + "-Misses:    ", headerWidth)
                  + mydecstr(Misses(accessType), numberWidth) +
                  "  " +fltstr(100.0 * Misses(accessType) / Accesses(accessType), 2, 6) + "%\n";
        
           out += prefix + ljstr(type + "-Accesses:  ", headerWidth)
                  + mydecstr(Accesses(accessType), numberWidth) +
                  "  " +fltstr(100.0 * Accesses(accessType) / Accesses(accessType), 2, 6) + "%\n";
        
           out += prefix + "\n";
       }
    }

    out += prefix + ljstr("Total-Hits:      ", headerWidth)
           + mydecstr(Hits(), numberWidth) +
           "  " +fltstr(100.0 * Hits() / Accesses(), 2, 6) + "%\n";

    out += prefix + ljstr("Total-Misses:    ", headerWidth)
           + mydecstr(Misses(), numberWidth) +
           "  " +fltstr(100.0 * Misses() / Accesses(), 2, 6) + "%\n";

    out += prefix + ljstr("Total-Accesses:  ", headerWidth)
           + mydecstr(Accesses(), numberWidth) +
           "  " +fltstr(100.0 * Accesses() / Accesses(), 2, 6) + "%\n";
    out += "\n";

    return out;
}
