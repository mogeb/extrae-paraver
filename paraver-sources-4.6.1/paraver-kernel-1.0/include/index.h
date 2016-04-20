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
 | @last_commit: $Date: 2012/03/08 16:45:27 $
 | @version:     $Revision: 1.9 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef INDEX_H_INCLUDED
#define INDEX_H_INCLUDED

#include "paraverkerneltypes.h"

#include <map>

static PRV_UINT32 INDEX_STEP = 10000;

template <typename RecordType>
class Index
{
  public:
    Index( PRV_UINT32 step = INDEX_STEP );
    ~Index();

    void indexRecord( TRecordTime time, RecordType rec );
    bool findRecord( TRecordTime time, RecordType& record ) const;

  private:
    typedef std::map< TRecordTime, RecordType > TTraceIndex;

    PRV_UINT32 indexStep;
    TTraceIndex baseIndex;
    PRV_UINT32 counter;
};

#include "src/index.cpp"

#endif // INDEX_H_INCLUDED