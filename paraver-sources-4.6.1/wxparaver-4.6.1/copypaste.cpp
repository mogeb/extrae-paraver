/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                                  wxparaver                                *
 *              Paraver Trace Visualization and Analysis Tool                *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL$
 | @last_commit: $Date: 2015/05/13 10:33:20 $
 | @version:     $Revision: 1.35 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#include "copypaste.h"
#include "trace.h"
#include "filter.h"

//#include <iostream>
//using namespace std;
//IDEA undo copy, undo paste

gPasteWindowProperties* gPasteWindowProperties::pasteWindowProperties = NULL;


void gPasteWindowProperties::commonMenuSettings( )
{
  for ( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
    for ( int origin = TIMELINE; origin <= HISTOGRAM; origin++ )
      for ( int destiny = TIMELINE; destiny <= HISTOGRAM; destiny++ )
      {
        allowed[STR_PASTE][trace][origin][destiny] = true;
        allowed[STR_PASTE_DEFAULT_SPECIAL][trace][origin][destiny] = true;
        allowed[STR_PASTE_SPECIAL][trace][origin][destiny] = true;
      }

  // Timeline/histogram different Menu properties
  if ( timeline != NULL )
  {
  }
  else
  {
  }
}


void gPasteWindowProperties::commonTimeSettings( TRecordTime destinyEndTime )
{
  TRecordTime sourceBeginTime;
  int source;

  if ( timeline != NULL )
  {
    sourceBeginTime = timeline->GetMyWindow()->getWindowBeginTime();
    source          = TIMELINE;
  }
  else
  {
    sourceBeginTime = histogram->GetHistogram()->getBeginTime();
    source          = HISTOGRAM;
  }

  if ( sourceBeginTime > destinyEndTime )
  {
    for( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
      for( int destiny = TIMELINE; destiny <= HISTOGRAM; destiny++ )
        allowed[STR_TIME][trace][source][destiny] = false;
  }
  else
  {
    for( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
      for( int destiny = TIMELINE; destiny <= HISTOGRAM; destiny++ )
        allowed[STR_TIME][trace][source][destiny] = true;
  }
}


void gPasteWindowProperties::commonFilterSettings( gTimeline *whichTimeline )
{
  if ( timeline != NULL )
  {
    if ( timeline->GetMyWindow()->isDerivedWindow() ||
         whichTimeline ->GetMyWindow()->isDerivedWindow() )
    {
      for( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
      {
        allowed[STR_FILTER][trace][TIMELINE][TIMELINE] = false;
        allowed[STR_FILTER_ALL][trace][TIMELINE][TIMELINE] = false;
        allowed[STR_FILTER_COMMS][trace][TIMELINE][TIMELINE] = false;
        allowed[STR_FILTER_EVENTS][trace][TIMELINE][TIMELINE] = false;
      }
    }
    else
    {
      for( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
      {
        allowed[STR_FILTER][trace][TIMELINE][TIMELINE] = true;
        allowed[STR_FILTER_ALL][trace][TIMELINE][TIMELINE] = true;
        allowed[STR_FILTER_COMMS][trace][TIMELINE][TIMELINE] = true;
        allowed[STR_FILTER_EVENTS][trace][TIMELINE][TIMELINE] = true;
      }
    }
  }
}


bool gPasteWindowProperties::seekAllowed( const string property, int destiny, Trace *destinyTrace )
{
  if ( timeline != NULL )
  {
    if ( timeline->GetMyWindow()->getTrace() == destinyTrace ||
         ( property == STR_OBJECTS &&
           timeline->GetMyWindow()->getTrace()->isSameObjectStruct( destinyTrace ) )
       )
      return allowed[property][SAME_TRACE][TIMELINE][destiny];
    else
      return allowed[property][DIFF_TRACE][TIMELINE][destiny];
  }
  else
  {
    if ( histogram->GetHistogram()->getControlWindow()->getTrace() == destinyTrace ||
         ( property == STR_OBJECTS &&
           histogram->GetHistogram()->getControlWindow()->getTrace()->isSameObjectStruct( destinyTrace ) )
       )
      return allowed[property][SAME_TRACE][HISTOGRAM][destiny];
    else
      return allowed[property][DIFF_TRACE][HISTOGRAM][destiny];
  }
}


gPasteWindowProperties::gPasteWindowProperties()
{
  timeline = NULL;
  histogram = NULL;

  vector< bool > destiny( 2 , false );
  vector < vector< bool > > source( 2, destiny );
  vector< vector < vector< bool > > > option( 2, source );

  // Policy : Allow paste option between different kind of windows and different traces
  for ( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
    for ( int copy = TIMELINE; copy <= HISTOGRAM; copy++ )
      for ( int paste = TIMELINE; paste <= HISTOGRAM; paste++ )
        option[trace][copy][paste] = true;

  allowed[STR_TIME] = option;
  allowed[STR_COPY] = option;
  allowed[STR_SIZE] = option;
  allowed[STR_DURATION] = option;
  allowed[STR_SEMANTIC_SCALE] = option;

  // Policy : Only same trace
  for ( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
    for ( int copy = TIMELINE; copy <= HISTOGRAM; copy++ )
      for ( int paste = TIMELINE; paste <= HISTOGRAM; paste++ )
        if ( trace == SAME_TRACE )
          option[trace][copy][paste] = true;
        else
          option[trace][copy][paste] = false;
  allowed[STR_OBJECTS] = option;

  // Policy : Don't allow initial paste
  for ( int trace = SAME_TRACE; trace <= DIFF_TRACE; trace++ )
    for ( int copy = TIMELINE; copy <= HISTOGRAM; copy++ )
      for ( int paste = TIMELINE; paste <= HISTOGRAM; paste++ )
        option[trace][copy][paste] = false;
  allowed[STR_PASTE] = option;
  allowed[STR_PASTE_DEFAULT_SPECIAL] = option;
  allowed[STR_PASTE_SPECIAL] = option;

  // Policy : Selective paste for FILTER options
  option[SAME_TRACE][TIMELINE][TIMELINE] = true;
  option[SAME_TRACE][TIMELINE][HISTOGRAM] = false;
  option[SAME_TRACE][HISTOGRAM][TIMELINE] = false;
  option[SAME_TRACE][HISTOGRAM][HISTOGRAM] = false;
  option[DIFF_TRACE][TIMELINE][TIMELINE] = true;
  option[DIFF_TRACE][TIMELINE][HISTOGRAM] = false;
  option[DIFF_TRACE][HISTOGRAM][TIMELINE] = false;
  option[DIFF_TRACE][HISTOGRAM][HISTOGRAM] = false;

  allowed[STR_FILTER] = option;
  allowed[STR_FILTER_ALL] = option;
  allowed[STR_FILTER_COMMS] = option;
  allowed[STR_FILTER_EVENTS] = option;
  
  option[SAME_TRACE][TIMELINE][TIMELINE] = false;
  option[SAME_TRACE][TIMELINE][HISTOGRAM] = false;
  option[SAME_TRACE][HISTOGRAM][TIMELINE] = false;
  option[SAME_TRACE][HISTOGRAM][HISTOGRAM] = true;
  option[DIFF_TRACE][TIMELINE][TIMELINE] = false;
  option[DIFF_TRACE][TIMELINE][HISTOGRAM] = false;
  option[DIFF_TRACE][HISTOGRAM][TIMELINE] = false;
  option[DIFF_TRACE][HISTOGRAM][HISTOGRAM] = true;
  
  allowed[STR_CONTROL_SCALE] = option;
  allowed[STR_3D_SCALE] = option;
}


gPasteWindowProperties::~gPasteWindowProperties()
{
}


gPasteWindowProperties *gPasteWindowProperties::getInstance()
{
  if ( gPasteWindowProperties::pasteWindowProperties == NULL )
    gPasteWindowProperties::pasteWindowProperties = new gPasteWindowProperties();
  return gPasteWindowProperties::pasteWindowProperties;
}


void gPasteWindowProperties::copy( gTimeline* whichTimeline )
{
  timeline = whichTimeline;
  histogram = NULL;
}


void gPasteWindowProperties::copy( gHistogram* whichHistogram )
{
  timeline = NULL;
  histogram = whichHistogram;
}


void gPasteWindowProperties::paste( gTimeline* whichTimeline,const string property )
{
  if ( timeline != NULL )
  {
    // paste timeline -> whichtimeline
    if ( property == STR_TIME )
    {
      TRecordTime sourceBeginTime = timeline->GetMyWindow()->getWindowBeginTime();
      TRecordTime sourceEndTime   = timeline->GetMyWindow()->getWindowEndTime();

      sourceBeginTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceBeginTime, timeline->GetMyWindow()->getTrace()->getTimeUnit() );
      sourceEndTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceEndTime, timeline->GetMyWindow()->getTrace()->getTimeUnit() );
      
      whichTimeline->GetMyWindow()->setWindowBeginTime( sourceBeginTime );
      whichTimeline->GetMyWindow()->setWindowEndTime( sourceEndTime );
    }
    else if ( property == STR_SIZE )
    {
      int width, height;
      width = timeline->GetMyWindow()->getWidth();
      height = timeline->GetMyWindow()->getHeight();
      whichTimeline->resizeDrawZone( width, height );
    }
    else if ( property == STR_OBJECTS )
    {
      vector< bool > auxRows;
      int lastLevel;
      if( whichTimeline->GetMyWindow()->getTrace()->existResourceInfo() && 
          timeline->GetMyWindow()->getTrace()->existResourceInfo() )
        lastLevel = CPU;
      else
        lastLevel = THREAD;
      for( int iLevel = APPLICATION; iLevel <= lastLevel; ++iLevel )
      {
        if( iLevel == SYSTEM )
          continue;
        timeline->GetMyWindow()->getSelectedRows( (TWindowLevel)iLevel, auxRows, true );
        whichTimeline->GetMyWindow()->setSelectedRows( (TWindowLevel)iLevel, auxRows );
      }
      whichTimeline->GetMyWindow()->addZoom( whichTimeline->GetMyWindow()->getWindowBeginTime(),
                                             whichTimeline->GetMyWindow()->getWindowEndTime(),
                                             timeline->GetMyWindow()->getZoomSecondDimension().first,
                                             timeline->GetMyWindow()->getZoomSecondDimension().second );
    }
    else if ( property == STR_DURATION )
    {
      TRecordTime sourceBeginTime = timeline->GetMyWindow()->getWindowBeginTime();
      TRecordTime sourceEndTime   = timeline->GetMyWindow()->getWindowEndTime();

      sourceBeginTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceBeginTime, timeline->GetMyWindow()->getTrace()->getTimeUnit() );
      sourceEndTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceEndTime, timeline->GetMyWindow()->getTrace()->getTimeUnit() );

      TRecordTime sourceDuration  = sourceEndTime - sourceBeginTime;
      TRecordTime newEndTime      = whichTimeline->GetMyWindow()->getWindowBeginTime() + sourceDuration;

      whichTimeline->GetMyWindow()->setWindowEndTime( newEndTime );
    }
    else if ( property == STR_FILTER_COMMS )
    {
      whichTimeline->GetMyWindow()->getFilter()->copyCommunicationsSection( timeline->GetMyWindow()->getFilter() );
    }
    else if ( property == STR_FILTER_EVENTS )
    {
      whichTimeline->GetMyWindow()->getFilter()->copyEventsSection( timeline->GetMyWindow()->getFilter() );
    }
    else if ( property == STR_FILTER_ALL )
    {
      whichTimeline->GetMyWindow()->getFilter()->copyEventsSection( timeline->GetMyWindow()->getFilter() );
      whichTimeline->GetMyWindow()->getFilter()->copyCommunicationsSection( timeline->GetMyWindow()->getFilter() );
    }
    else if ( property == STR_SEMANTIC_SCALE )
    {
      whichTimeline->GetMyWindow()->setMinimumY( timeline->GetMyWindow()->getMinimumY() );
      whichTimeline->GetMyWindow()->setMaximumY( timeline->GetMyWindow()->getMaximumY() );
    }
    else
    {
    }
  }
  else
  {
    // paste histogram -> timeline
    if ( property == STR_TIME )
    {
      TRecordTime sourceBeginTime = histogram->GetHistogram()->getBeginTime();
      TRecordTime sourceEndTime   = histogram->GetHistogram()->getEndTime();

      sourceBeginTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceBeginTime, histogram->GetHistogram()->getTrace()->getTimeUnit() );
      sourceEndTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceEndTime, histogram->GetHistogram()->getTrace()->getTimeUnit() );

      whichTimeline->GetMyWindow()->setWindowBeginTime( sourceBeginTime );
      whichTimeline->GetMyWindow()->setWindowEndTime( sourceEndTime );
    }
    else if ( property == STR_SIZE )
    {
      int width, height;
      histogram->GetSize( &width, &height);
      whichTimeline->SetSize( width, height );
    }
    else if ( property == STR_OBJECTS )
    {
      vector< bool > auxRows;
      Window *controlWin = histogram->GetHistogram()->getControlWindow();
      controlWin->getSelectedRows( controlWin->getLevel(), auxRows, true );
      whichTimeline->GetMyWindow()->setSelectedRows( whichTimeline->GetMyWindow()->getLevel(), auxRows );
    }
    else if ( property == STR_DURATION )
    {
      TRecordTime sourceBeginTime = histogram->GetHistogram()->getBeginTime();
      TRecordTime sourceEndTime   = histogram->GetHistogram()->getEndTime();

      sourceBeginTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceBeginTime, histogram->GetHistogram()->getTrace()->getTimeUnit() );
      sourceEndTime = whichTimeline->GetMyWindow()->customUnitsToTraceUnits( sourceEndTime, histogram->GetHistogram()->getTrace()->getTimeUnit() );

      TRecordTime sourceDuration  = sourceEndTime - sourceBeginTime;
      TRecordTime newEndTime      = whichTimeline->GetMyWindow()->getWindowBeginTime() + sourceDuration;

      whichTimeline->GetMyWindow()->setWindowEndTime( newEndTime );
    }
    else if ( property == STR_SEMANTIC_SCALE )
    {
      Window *controlWin = histogram->GetHistogram()->getControlWindow();
      whichTimeline->GetMyWindow()->setMinimumY( controlWin->getMinimumY() );
      whichTimeline->GetMyWindow()->setMaximumY( controlWin->getMaximumY() );
    }
    else
    {
      cout << "*** UNKNOWN PROPERTY!!! ***" << endl;
    }
  }
}


void gPasteWindowProperties::paste( gHistogram* whichHistogram, const string property )
{
  if ( timeline != NULL )
  {
    // paste timeline -> histogram
    if ( property == STR_TIME )
    {
      whichHistogram->GetHistogram()->setWindowBeginTime( timeline->GetMyWindow()->getWindowBeginTime() );
      whichHistogram->GetHistogram()->setWindowEndTime( timeline->GetMyWindow()->getWindowEndTime() );
    }
    else if ( property == STR_SIZE )
    {
      int width, height;
      timeline->GetSize( &width, &height);
      whichHistogram->SetSize( width, height );
    }
    else if ( property == STR_OBJECTS )
    {
      vector< bool > auxRows;
      timeline->GetMyWindow()->getSelectedRows( timeline->GetMyWindow()->getLevel(), auxRows, true );
      Window *controlWindow = whichHistogram->GetHistogram()->getControlWindow();
      controlWindow->setSelectedRows( controlWindow->getLevel(), auxRows );
      /*controlWindow->addZoom( controlWindow->getWindowBeginTime(),
                              controlWindow->getWindowEndTime(),
                              timeline->GetMyWindow()->getZoomSecondDimension().first,
                              timeline->GetMyWindow()->getZoomSecondDimension().second );*/
      std::pair<TObjectOrder, TObjectOrder> tmpZoom( timeline->GetMyWindow()->getZoomSecondDimension().first,
                                                     timeline->GetMyWindow()->getZoomSecondDimension().second );
      whichHistogram->GetHistogram()->setZoomSecondDimension( tmpZoom );
    }
    else if ( property == STR_DURATION )
    {
      TRecordTime sourceBeginTime = timeline->GetMyWindow()->getWindowBeginTime();
      TRecordTime sourceEndTime   = timeline->GetMyWindow()->getWindowEndTime();
      TRecordTime sourceDuration  = sourceEndTime - sourceBeginTime;
      TRecordTime newEndTime      = whichHistogram->GetHistogram()->getBeginTime() + sourceDuration;

      whichHistogram->GetHistogram()->setWindowEndTime( newEndTime );
    }
    else if ( property == STR_SEMANTIC_SCALE )
    {
      whichHistogram->GetHistogram()->setMinGradient( timeline->GetMyWindow()->getMinimumY() );
      whichHistogram->GetHistogram()->setMaxGradient( timeline->GetMyWindow()->getMaximumY() );
    }
    else
    {
    }
  }
  else
  {
    // paste histogram -> histogram
    if ( property == STR_TIME )
    {
      whichHistogram->GetHistogram()->setWindowBeginTime( histogram->GetHistogram()->getBeginTime() );
      whichHistogram->GetHistogram()->setWindowEndTime( histogram->GetHistogram()->getEndTime() );
    }
    else if ( property == STR_SIZE )
    {
      int width, height;
      histogram->GetSize( &width, &height);
      whichHistogram->SetSize( width, height );
    }
    else if ( property == STR_OBJECTS )
    {
      vector< bool > auxRows;
      histogram->GetHistogram()->getControlWindow()->getSelectedRows( 
          histogram->GetHistogram()->getControlWindow()->getLevel(), auxRows, true );
      Window *controlWindow = whichHistogram->GetHistogram()->getControlWindow();
      controlWindow->setSelectedRows( controlWindow->getLevel(), auxRows );
      controlWindow->addZoom( controlWindow->getWindowBeginTime(),
                              controlWindow->getWindowEndTime(),
                              histogram->GetHistogram()->getControlWindow()->getZoomSecondDimension().first,
                              histogram->GetHistogram()->getControlWindow()->getZoomSecondDimension().second );
    }
    else if ( property == STR_DURATION )
    {
      TRecordTime sourceBeginTime = histogram->GetHistogram()->getBeginTime();
      TRecordTime sourceEndTime   = histogram->GetHistogram()->getEndTime();
      TRecordTime sourceDuration  = sourceEndTime - sourceBeginTime;
      TRecordTime newEndTime      = whichHistogram->GetHistogram()->getBeginTime() + sourceDuration;

      whichHistogram->GetHistogram()->setWindowEndTime( newEndTime );
    }
    else if ( property == STR_SEMANTIC_SCALE )
    {
      whichHistogram->GetHistogram()->setMinGradient( histogram->GetHistogram()->getMinGradient() );
      whichHistogram->GetHistogram()->setMaxGradient( histogram->GetHistogram()->getMaxGradient() );
    }
    else if( property == STR_CONTROL_SCALE )
    {
      Histogram *srcHisto = histogram->GetHistogram();
      Histogram *dstHisto = whichHistogram->GetHistogram();
      dstHisto->setControlMin( srcHisto->getControlMin() );
      dstHisto->setControlMax( srcHisto->getControlMax() );
      dstHisto->setControlDelta( srcHisto->getControlDelta() );
    }
    else if( property == STR_3D_SCALE )
    {
      Histogram *srcHisto = histogram->GetHistogram();
      Histogram *dstHisto = whichHistogram->GetHistogram();
      if( dstHisto->getExtraControlDelta() == srcHisto->getExtraControlDelta() )
      {
        double planeMin = dstHisto->getPlaneMinValue();
        dstHisto->setExtraControlDelta( srcHisto->getExtraControlDelta() );
        dstHisto->setPlaneMinValue( planeMin );
      }
      else
        dstHisto->setExtraControlDelta( srcHisto->getExtraControlDelta() );
      dstHisto->setExtraControlMin( srcHisto->getExtraControlMin() );
      dstHisto->setExtraControlMax( srcHisto->getExtraControlMax() );
    }
    else
    {
    }
  }
}


bool gPasteWindowProperties::isAllowed( gTimeline *whichTimeline, const string property )
{
  if ( timeline == NULL && histogram == NULL )
    return false;

  /*if ( property == STR_TIME )
    commonTimeSettings( whichTimeline->GetMyWindow()->getTrace()->getEndTime() );*/
  
  if ( property == STR_FILTER ||
       property == STR_FILTER_ALL ||
       property == STR_FILTER_EVENTS ||
       property == STR_FILTER_COMMS )
    commonFilterSettings( whichTimeline );

  commonMenuSettings();

  return seekAllowed( property, TIMELINE, whichTimeline->GetMyWindow()->getTrace() );
}


bool gPasteWindowProperties::isAllowed( gHistogram *whichHistogram, const string property )
{

  if ( timeline == NULL && histogram == NULL )
    return false;

  if ( property == STR_TIME )
    commonTimeSettings( whichHistogram->GetHistogram()->getControlWindow()->getTrace()->getEndTime() );
    
  commonMenuSettings();

  return seekAllowed( property, HISTOGRAM, whichHistogram->GetHistogram()->getControlWindow()->getTrace() );
}


TRecordTime gPasteWindowProperties::getBeginTime()
{
  if ( timeline != NULL )
    return timeline->GetMyWindow()->getWindowBeginTime();

  return histogram->GetHistogram()->getBeginTime();
}

TRecordTime gPasteWindowProperties::getEndTime()
{
  if ( timeline != NULL )
    return timeline->GetMyWindow()->getWindowEndTime();

  return histogram->GetHistogram()->getEndTime();
}

void gPasteWindowProperties::verifyRemove( gTimeline *whichTimeline )
{
  if( whichTimeline == timeline )
    timeline = NULL;
}

void gPasteWindowProperties::verifyRemove( gHistogram *whichHistogram )
{
  if( whichHistogram == histogram )
    histogram = NULL;
}
