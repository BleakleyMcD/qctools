/*  Copyright (c) BAVC. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "Core/CommonStats.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
extern "C"
{
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <libavutil/frame.h>
}

#include "Core/Core.h"
#include "tinyxml2.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cfloat>
using namespace tinyxml2;
//---------------------------------------------------------------------------

//***************************************************************************
// Constructor / Destructor
//***************************************************************************

//---------------------------------------------------------------------------
CommonStats::CommonStats (const struct per_item* PerItem_, int Type_, size_t CountOfGroups_, size_t CountOfItems_, size_t FrameCount, double Duration, size_t FrameCount_Max, double Frequency_)
    :
    Frequency(Frequency_),
    PerItem(PerItem_),
    Type(Type_),
    CountOfGroups(CountOfGroups_),
    CountOfItems(CountOfItems_)
{
    // Adaptation for having a graph even with 1 frame
    if (FrameCount<2)
        FrameCount=2;
    if (FrameCount_Max<2)
        FrameCount_Max=2;

    // Status
    IsComplete=false;
    FirstTimeStamp=DBL_MAX;

    // Memory management
    Data_Reserved=FrameCount_Max;

    // Data - Counts
    Stats_Totals = new double[CountOfItems];
    memset(Stats_Totals, 0x00, CountOfItems*sizeof(double));
    Stats_Counts = new uint64_t[CountOfItems];
    memset(Stats_Counts, 0x00, CountOfItems*sizeof(uint64_t));
    Stats_Counts2 = new uint64_t[CountOfItems];
    memset(Stats_Counts2, 0x00, CountOfItems*sizeof(uint64_t));

    // Data - x and y
    x = new double*[4];
    for (size_t j=0; j<4; ++j)
    {
        x[j]=new double[FrameCount_Max];
        memset(x[j], 0x00, FrameCount_Max*sizeof(double));
    }
    y = new double*[CountOfItems];
    for (size_t j=0; j<CountOfItems; ++j)
    {
        y[j]=new double[FrameCount_Max];
        memset(y[j], 0x00, FrameCount_Max*sizeof(double));
    }

    // Data - Extra
    durations=new double[FrameCount_Max];
    memset(durations, 0x00, FrameCount_Max*sizeof(double));
    key_frames=new bool[FrameCount_Max];
    memset(key_frames, 0x00, FrameCount_Max*sizeof(bool));

    // Data - Maximums
    x_Current=0;
    x_Current_Max=FrameCount;
    x_Max[0]=x_Current_Max;
    x_Max[1]=Duration;
    x_Max[2]=x_Max[1]/60;
    x_Max[3]=x_Max[2]/60;
    y_Min=new double[CountOfGroups];
    memset(y_Min, 0x00, CountOfGroups*sizeof(double));
    y_Max=new double[CountOfGroups];
    memset(y_Max, 0x00, CountOfGroups*sizeof(double));
}

//---------------------------------------------------------------------------
CommonStats::~CommonStats()
{
    // Data - Counts
    delete[] Stats_Totals;
    delete[] Stats_Counts;
    delete[] Stats_Counts2;

    // Data - x and y
    for (size_t j=0; j<4; ++j)
        delete[] x[j];
    delete[] x;

    // Data - Extra
    for (size_t j=0; j<CountOfItems; ++j)
        delete[] y[j];
    delete[] y;

    // Data - Maximums
    delete[] durations;
    delete[] key_frames;
    delete[] y_Min;
    delete[] y_Max;
}

//***************************************************************************
// Status
//***************************************************************************

//---------------------------------------------------------------------------
int CommonStats::Type_Get()
{
    return Type;
}

//---------------------------------------------------------------------------
double CommonStats::State_Get()
{
    if (IsComplete || x_Current_Max==0)
        return 1;

    double Value=((double)x_Current)/x_Current_Max;
    if (Value>=1)
        Value=0.99; // It is not yet complete, so not 100%

    return Value;
}

//---------------------------------------------------------------------------
void CommonStats::StatsFinish ()
{
    // Adaptation
    if (x_Current==1)
    {
        x[0][1]=1;
        x[1][1]=durations[0]?durations[0]:1; //forcing to 1 in case duration is not available
        x[2][1]=durations[0]?(durations[0]/60):1; //forcing to 1 in case duration is not available
        x[3][1]=durations[0]?(durations[0]/3600):1; //forcing to 1 in case duration is not available
        for (size_t Plot_Pos=0; Plot_Pos<CountOfItems; Plot_Pos++)
            y[Plot_Pos][1]= y[Plot_Pos][0];
        x_Current++;
    }

    // Forcing max values to the last real ones, in case max values were estimated
    if (x_Current)
    {
        x_Max[0]=x[0][x_Current-1];
        if (x[1][x_Current-1])
        {
            x_Max[1]=x[1][x_Current-1];
            x_Max[2]=x[2][x_Current-1];
            x_Max[3]=x[3][x_Current-1];
        }
    }

    x_Current_Max=x_Current;
    IsComplete=true;
}

//***************************************************************************
// Stats
//***************************************************************************

//---------------------------------------------------------------------------
string CommonStats::Average_Get(size_t Pos)
{
    if (x_Current==0)
        return string();

    double Value=Stats_Totals[Pos]/x_Current;
    stringstream str;
    str<<fixed<<setprecision(PerItem[Pos].DigitsAfterComma)<<Value;
    return str.str();
}

//---------------------------------------------------------------------------
string CommonStats::Average_Get(size_t Pos, size_t Pos2)
{
    if (x_Current==0)
        return string();

    double Value=(Stats_Totals[Pos]-Stats_Totals[Pos2])/x_Current;
    stringstream str;
    str<<fixed<<setprecision(PerItem[Pos].DigitsAfterComma)<<Value;
    return str.str();
}

//---------------------------------------------------------------------------
string CommonStats::Count_Get(size_t Pos)
{
    if (x_Current==0)
        return string();

    stringstream str;
    str<<Stats_Counts[Pos];
    return str.str();
}

//---------------------------------------------------------------------------
string CommonStats::Count2_Get(size_t Pos)
{
    if (x_Current==0)
        return string();

    stringstream str;
    str<<Stats_Counts2[Pos];
    return str.str();
}

//---------------------------------------------------------------------------
string CommonStats::Percent_Get(size_t Pos)
{
    if (x_Current==0)
        return string();

    double Value=((double)Stats_Counts[Pos])/x_Current;
    stringstream str;
    str<<Value*100<<"%";
    return str.str();
}

//***************************************************************************
// Memory management
//***************************************************************************

//---------------------------------------------------------------------------
void CommonStats::Data_Reserve(size_t NewValue)
{
    // Saving old data
    size_t                      Data_Reserved_Old = Data_Reserved;
    double**                    x_Old = new double*[4];
    memcpy (x_Old, x, sizeof(double*)*4);
    double**                    y_Old = new double*[CountOfItems];
    memcpy (y_Old, y, sizeof(double*)*CountOfItems);
    double*                     durations_Old=durations;
    bool*                       key_frames_Old=key_frames;

    // Computing new value
    while (Data_Reserved<NewValue+(1<<18)) //We reserve extra space, minimum 2^18 frames added
        Data_Reserved<<=1;

    // Creating new data - x and y
    x = new double*[4];
    for (size_t j=0; j<4; ++j)
    {
        x[j]=new double[Data_Reserved];
        memset(x[j], 0x00, Data_Reserved*sizeof(double));
        memcpy(x[j], x_Old[j], Data_Reserved_Old*sizeof(double));
    }
    y = new double*[CountOfItems];
    for (size_t j=0; j<CountOfItems; ++j)
    {
        y[j] = new double[Data_Reserved];
        memset(y[j], 0x00, Data_Reserved*sizeof(double));
        memcpy(y[j], y_Old[j], Data_Reserved_Old*sizeof(double));
    }

    // Creating new data - Extra
    durations = new double[Data_Reserved];
    memset(durations, 0x00, Data_Reserved*sizeof(double));
    memcpy(durations, durations_Old, Data_Reserved_Old*sizeof(double));
    key_frames = new bool[Data_Reserved];
    memset(key_frames, 0x00, Data_Reserved*sizeof(bool));
    memcpy(key_frames, key_frames_Old, Data_Reserved_Old*sizeof(bool));
}
