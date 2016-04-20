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
 | @last_commit: $Date: 2012/04/02 16:04:37 $
 | @version:     $Revision: 1.7 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef PROCESSMODELAPPL_H_INCLUDED
#define PROCESSMODELAPPL_H_INCLUDED

#include <vector>
#include "processmodeltask.h"

class ProcessModelAppl
{

  public:
    ProcessModelAppl( TApplOrder order = 0 ): traceGlobalOrder( order )
    {}

    ~ProcessModelAppl()
    {}

    bool operator==( const ProcessModelAppl& other ) const
    {
      return traceGlobalOrder == other.traceGlobalOrder &&
             tasks            == other.tasks;
    }

  protected:
    TApplOrder traceGlobalOrder;
    std::vector<ProcessModelTask> tasks;

  private:
    friend class ProcessModel;

};

#endif // PROCESSMODELAPPL_H_INCLUDED