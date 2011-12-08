#include "eid.hpp"

EID generateNewEID(void)
{
    static EID nextEID = 1;
    return nextEID++;
}
