/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                                   Extrae                                  *
 *              Instrumentation package for parallel applications            *
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
 | @file: $HeadURL: https://svn.bsc.es/repos/ptools/extrae/tags/3.2/3.2.1/src/merger/paraver/misc_prv_semantics.h $
 | @last_commit: $Date: 2015-10-05 13:46:13 +0200 (Mon, 05 Oct 2015) $
 | @version:     $Revision: 3418 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef __MISC_PRV_SEMANTICS_H__
#define __MISC_PRV_SEMANTICS_H__

#include "record.h"
#include "semantics.h"
#include "file_set.h"

extern int MPI_Caller_Multiple_Levels_Traced;
extern int *MPI_Caller_Labels_Used;

extern int Sample_Caller_Multiple_Levels_Traced;
extern int *Sample_Caller_Labels_Used;

extern int Rusage_Events_Found;
extern int GetRusage_Labels_Used[RUSAGE_EVENTS_COUNT];

extern int Memusage_Events_Found;
extern int Memusage_Labels_Used[MEMUSAGE_EVENTS_COUNT];

extern int MPI_Stats_Events_Found;
extern int MPI_Stats_Labels_Used[MPI_STATS_EVENTS_COUNT];

extern SingleEv_Handler_t PRV_MISC_Event_Handlers[];
extern RangeEv_Handler_t PRV_MISC_Range_Handlers[];

#endif /* __MISC_PRV_SEMANTICS_H__ */
