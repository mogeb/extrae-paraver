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
 | @last_commit: $Date: 2012/03/07 16:15:55 $
 | @version:     $Revision: 1.4 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef SYNCWINDOWS_H_INCLUDED
#define SYNCWINDOWS_H_INCLUDED

#include "paraverkerneltypes.h"

class Window;

class SyncWindows
{
  public:
    static SyncWindows *getInstance();
    ~SyncWindows();

    bool addWindow( Window *whichWindow, unsigned int whichGroup = 0 );
    void removeWindow( Window *whichWindow, unsigned int whichGroup = 0 );
    void removeAll( unsigned int whichGroup = 0 );

    int newGroup();
    int getNumGroups() const;
    void broadcastTime( unsigned int whichGroup, Window *sendWindow, TTime beginTime, TTime endTime );

  private:
    SyncWindows();

    static SyncWindows *instance;
    std::vector<std::vector<Window *> > syncGroups;
    bool removingAll;
};

#endif // SYNCWINDOWS_H_INCLUDED
