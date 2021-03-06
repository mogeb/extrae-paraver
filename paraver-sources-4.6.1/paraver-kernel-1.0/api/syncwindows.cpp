/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                       Paraver Main Computing Library                      *
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
 | @last_commit: $Date: 2014/07/24 13:19:09 $
 | @version:     $Revision: 1.7 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#include "syncwindows.h"
#include "window.h"

using std::vector;

SyncWindows *SyncWindows::instance = NULL;

SyncWindows *SyncWindows::getInstance()
{
  if( SyncWindows::instance == NULL )
    SyncWindows::instance = new SyncWindows();
  return SyncWindows::instance;
}

SyncWindows::SyncWindows()
{
  syncGroups.push_back( vector<Window *>() );
  removingAll = false;
}

SyncWindows::~SyncWindows()
{
}

bool SyncWindows::addWindow( Window *whichWindow, unsigned int whichGroup )
{
  if( whichGroup >= syncGroups.size() )
    return false;

  syncGroups[ whichGroup ].push_back( whichWindow );
  if( syncGroups[ whichGroup ].size() > 1 &&
      ( whichWindow->getWindowBeginTime() != syncGroups[ whichGroup ][ 0 ]->getWindowBeginTime() ||
        whichWindow->getWindowEndTime() != syncGroups[ whichGroup ][ 0 ]->getWindowEndTime() )
    )
  {
    TTime nanoBeginTime, nanoEndTime;
    nanoBeginTime = syncGroups[ whichGroup ][ 0 ]->traceUnitsToCustomUnits( syncGroups[ whichGroup ][ 0 ]->getWindowBeginTime(), NS );
    nanoEndTime = syncGroups[ whichGroup ][ 0 ]->traceUnitsToCustomUnits( syncGroups[ whichGroup ][ 0 ]->getWindowEndTime(), NS );
    whichWindow->setWindowBeginTime( whichWindow->customUnitsToTraceUnits( nanoBeginTime, NS ), true );
    whichWindow->setWindowEndTime( whichWindow->customUnitsToTraceUnits( nanoEndTime, NS ), true );
    whichWindow->setChanged( true );
    whichWindow->setRedraw( true );
  }

  return true;
}

void SyncWindows::removeWindow( Window *whichWindow, unsigned int whichGroup )
{
  if( whichGroup >= syncGroups.size() || removingAll )
    return;

  for( vector<Window *>::iterator it = syncGroups[ whichGroup ].begin();
       it != syncGroups[ whichGroup ].end(); ++it )
  {
    if( *it == whichWindow )
    {
      syncGroups[ whichGroup ].erase( it );
      break;
    }
  }
}

void SyncWindows::removeAll( unsigned int whichGroup )
{
  if( whichGroup >= syncGroups.size() )
    return;

  removingAll = true;

  for( vector<Window *>::iterator it = syncGroups[ whichGroup ].begin();
       it != syncGroups[ whichGroup ].end(); ++it )
    (*it)->removeFromSync();

  syncGroups[ whichGroup ].clear();
  removingAll = false;
}

int SyncWindows::newGroup()
{
  syncGroups.push_back( vector<Window *>() );
  return syncGroups.size();
}

int SyncWindows::getNumGroups() const
{
  return syncGroups.size();
}

void SyncWindows::broadcastTime( unsigned int whichGroup, Window *sendWindow, TTime beginTime, TTime endTime )
{
  for( vector<Window *>::iterator it = syncGroups[ whichGroup ].begin();
       it != syncGroups[ whichGroup ].end(); ++it )
  {
    TTime tmpBeginTime, tmpEndTime;
    tmpBeginTime = ( *it )->customUnitsToTraceUnits( beginTime, NS );
    tmpEndTime = ( *it )->customUnitsToTraceUnits( endTime, NS );
    if( ( *it ) != sendWindow  &&
        ( ( *it )->getWindowBeginTime() != tmpBeginTime ||
          ( *it )->getWindowEndTime() != tmpEndTime )
      )
    {
      ( *it )->addZoom( tmpBeginTime, tmpEndTime, true );
      ( *it )->setWindowBeginTime( tmpBeginTime, true );
      ( *it )->setWindowEndTime( tmpEndTime, true );
      ( *it )->setChanged( true );
      ( *it )->setRedraw( true );
    }
  }
}

