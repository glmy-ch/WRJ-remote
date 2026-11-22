#include "Com_tool.h"
uint16_t COM_Tool( int16_t *data, uint16_t min, uint16_t max)
{
    if(*data<min)
    {
        return min;
    }
    if(*data > max)
    {
        return max;
    }
    return *data;
}
