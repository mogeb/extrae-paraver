/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                          paraver-toolset::stats                           *
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
 | @last_commit: $Date$
 | @version:     $Revision$
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#define MAX_CPUS 16384


struct cpu_info
{
  int id;
  unsigned long long time;
  struct cpu_info *next_cpu;
};



struct call_info
{
  unsigned long long type;
  unsigned long long value;
  unsigned long long ini_time;
  unsigned long long time;
  int in_call;
  unsigned long long num;
  struct call_info *next_call;
};



struct comm_info
{
  int task_rcv;
  int size;
  int num_comm;
  struct comm_info *next_comm;

};



struct thread_info
{
  int appl;
  int task;
  int thread;
  unsigned long long running_time;
  unsigned long long total_time;
  struct cpu_info *cpus;
  struct call_info *calls;
  struct call_info *stack[20];
  int top;
  struct comm_info *comm_counter;
};


struct pcf_label
{
  unsigned long long id;
  char *name;
};


struct type
{
  unsigned long long id;
  char *name;
  struct pcf_label values[200];
  int next_value;
};


#define MAX_BURST_CLASS 16

struct histo_bursts
{
  unsigned long long total_run_time;
  unsigned long long burst_class[MAX_BURST_CLASS];
  unsigned long long n_burst_class[MAX_BURST_CLASS];

  /* 0 < x <= 1 us */
  /* 1 us < x <= 5 us */
  /* 5 us < x <= 10 us */
  /* 10 us < x <= 50 us */
  /* 50 us < x <= 100 us */
  /* 100 us < x <= 500 us */
  /* 500 us < x <= 1 ms */
  /* 1 ms < x <= 5 ms */
  /* 5 ms < x <= 10 ms */
  /* 10 ms < x <= 50 ms */
  /* 50 ms < x <= 100 ms */
  /* 100 ms < x <= 500 ms */
  /* 500 ms < x <= 1 s */
  /* 1 s < x <= 5 s */
  /* 5 s < x <= 10 s */
  /* 10 s < x */

};

#define MAX_COMMS_CLASS 8

struct histo_comms
{
  unsigned long long total_bytes_send;

  unsigned long long comms_class[MAX_COMMS_CLASS];
  unsigned long long n_comms_class[MAX_COMMS_CLASS];

  /* <= 10 bytes */
  /* 10 b < x <= 100 b */
  /* 100 b < x <= 1Kb */
  /* 1 Kb < x <= 10 Kb */
  /* 100 Kb < x <= 100 Kb */
  /* 100 Kb < x <= 1 Mb */
  /* 1 Mb < x <= 10 Mb */
  /* 10 Mb < x */
};


#define MAX_HISTO_EVENTS 100

struct histo_events
{
  unsigned long long types[MAX_HISTO_EVENTS];
  unsigned long long counters[MAX_HISTO_EVENTS];
  int last_free_slot;
};
