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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#include "stats.h"

#define MAX_HEADER_SIZE 131072

/* Buffer for reading trace records */
char line[32768];


/* Execution parameters */
int rank_min, rank_max;

int rank_threads         = 0,
    calls_exclusive_time = 0;

int show_cpus             = 0,
    show_bursts_histo     = 0,
    show_comms_histo      = 0,
    show_events_histo     = 0,
    show_thread_resources = 0,
    show_thread_calls     = 0,
    show_comms            = 0,
    only_dat_file         = 0;

char *new_file_name, *logfile;


/* Trace in and trace out */
FILE *infile, *outfile;

/* Pointers to the thread struct, for avoiding to much searches */
int thread_pointer[2][12000][10];


/* Structs */

struct thread_info threads[12000];
int next_thread_slot;

struct type types[100];
int next_type;
unsigned long long cpus[MAX_CPUS];
unsigned long long trace_time;

struct histo_bursts bursts_histo;

struct histo_comms comms_histo;

struct histo_events events_histo;


struct types
{
  unsigned long long min_type;
  unsigned long long max_type;
  int ranged_type;
};



struct types allowed_types[20];
int next_allowed_type = 0;


/* Parameters for showing percentage */
unsigned long long total_trace_size, current_readed_size;
double last_showed = 0;
unsigned long total_iters = 0;

void
print_usage(char* binary_name)
{
  printf
  ("\nApplication for obtaining various statistics from Paraver traces\n\n");
  printf ("Usage:  %s < trace_in > < options > \n\n", binary_name);
  printf ("Options:\n");
  printf ("  --help                            Display this info\n");
  printf
  ("  -o < file_name >                  Generate stats in file <file_name>.dat and \n");
  printf
  ("                                    <file_name>.gnuplot. By default is generated\n");
  printf
  ("                                    in file <trace_in>.dat & <trace_in>.gnuplot\n");
  printf
  ("  -only_dat_file                    In conjunction with the \"-o\" flag, generates\n");
  printf
  ("                                    only the data file with name <file_name>\n");
  printf ("  -bursts_histo                     Show bursts histogram\n");
  printf ("  -comms_histo                      Show comms histogram\n");
  printf
  ("  -events_histo[:type1[-type2],...] Show events histogram, filtering types or\n");
  printf ("                                    range of types if needed\n");
  printf
  ("  -thread_calls[:type1[-type2],...] Shows stats of the calls made by thread, \n");
  printf
  ("                                    filtering by type or range of types. This\n");
  printf
  ("                                    info is generated in file <trace_in>.calls.info\n");
  printf
  ("  -exclusive_times                  Calculate exclusive times of routine calls\n");
  printf ("                                    instead inclusive times\n");
}

/* Function for parsing program parameters */
void
read_args (int argc, char *argv[])
{
  int i, new_name = 0;
  char *word, *types_allowed, *word2;

  if (argc < 3)
  {
    print_usage(argv[0]);
    exit (0);
  }

  /*
      if(argc < 4) {
        printf("\nApplication for obtaining various statistics from Paraver traces\n\n");
        printf("Usage:  %s < trace_in > < stats_file > < options >\n\n",argv[0]);
        printf("Options:\n");
        printf("  --help                            Display this info\n");
        printf("  -system_cpus                      Shows %% of all the CPU's ocuppation\n");
        printf("  -thread_cpus                      Shows %% of CPU ocuppation per thread\n");
        printf("  -task_comms                       Shows info of the communications\n");
        printf("  -thread_calls[:type1[-type2],...] Shows stats of the calls made by thread, \n");
        printf("                                    filtering by type or range of types\n");
        printf("  -tasks <n1-n2>                    Show the info only for the tasks between \n");
        printf("                                    n1-n2\n");

        printf("\n Examples:\n\t%s t1.prv stats.dat -task_comms -thread_calls:2000,5000001-500003\n\n",argv[0]);
        exit(0);
      }
   */

  if (argv[1][0] == '-')
  {
    printf ("\n\nERROR: First argument has to be a Paraver Trace\n\n");
    exit (-1);
  }


  new_file_name = strdup (argv[1]);

  i = 2;

  while (i < argc)
  {

    if (!strcmp (argv[i], "-o"))
    {
      new_name = 1;
      free (new_file_name);
      new_file_name = strdup (argv[i + 1]);
      i += 2;
      continue;
    }

    if (!strcmp (argv[i], "-only_dat_file"))
    {
      only_dat_file = 1;
      i++;
      continue;
    }

    if (!strcmp (argv[i], "-task_comms"))
    {
      show_comms = 1;
      i++;
      continue;
    }

    if (!strcmp (argv[i], "-system_cpus"))
    {
      show_cpus = 1;
      i++;
      continue;
    }

    if (!strcmp (argv[i], "-bursts_histo"))
    {
      show_bursts_histo = 1;
      i++;
      continue;
    }

    if (!strcmp (argv[i], "-comms_histo"))
    {
      show_comms_histo = 1;
      i++;
      continue;
    }

    if (!strcmp (argv[i], "-thread_cpus"))
    {
      show_thread_resources = 1;
      i++;
      continue;
    }

    if (strstr (argv[i], "-thread_calls") ||
        strstr (argv[i], "-events_histo"))
    {

      if (strstr (argv[i], "-thread_calls"))
      {
        show_thread_calls = 1;
        logfile = (char *) malloc (1024);
        strcpy (logfile, argv[1]);
        logfile[strlen (logfile) - 4] = '\0';
        sprintf (logfile, "%s.calls.info", logfile);
      }
      else
        show_events_histo = 1;


      word = strtok (argv[i], ":");

      if ((types_allowed = strtok (NULL, ":")) != NULL)
      {
        word = strtok (types_allowed, ",");

        do
        {
          if ((word2 = strchr (word, '-')) != NULL)
          {
            *word2 = '\0';
            allowed_types[next_allowed_type].min_type = atoll (word);
            allowed_types[next_allowed_type].max_type = atoll (++word2);
            allowed_types[next_allowed_type].ranged_type = 1;

          }
          else
          {
            allowed_types[next_allowed_type].min_type = atoll (word);
            allowed_types[next_allowed_type].ranged_type = 0;
          }

          next_allowed_type++;
        }
        while ((word = strtok (NULL, ",")) != NULL);
      }

      i++;

      continue;
    }

    if (!strcmp (argv[i], "-exclusive_times"))
    {
      calls_exclusive_time = 1;
      i++;
      continue;
    }

    if (!strcmp (argv[i], "-tasks"))
    {

      rank_threads = 1;
      word = strtok (argv[i + 1], "-");
      rank_min = atoi (word);
      word = strtok (NULL, "-");
      rank_max = atoi (word);
      i += 2;
      continue;

    }

    printf ("\n\nERROR: unknwon parameter %s\n\n", argv[i]);

    print_usage(argv[0]);
    exit(-1);

  }

  if (only_dat_file && !new_name)
  {
    printf
    ("\n\nERROR: The flag \"-only_dat_file\" can only be specified in conjunction with the \"-o\" flag\n\n");
    exit (0);
  }
}



/* For processing the Paraver header */
void
proces_header (char *header)
{
  int num_comms;
  char *word;


  /* Obtaining the total trace time */
  word = strtok (header, ":");
  word = strtok (NULL, ":");
  word = strtok (NULL, ":");
  word[strlen (word) - 3] = '\0';
  trace_time = atoll (word);



  word = strtok (NULL, "\n");
  word = strrchr (word, ',');


  /* Obtaining th number of communicators */
  strcpy (line, word + 1);

  if (strchr (line, ')') != NULL)
    return;

  num_comms = atoi (line);


  while (num_comms > 0)
  {
    fgets (header, MAX_HEADER_SIZE, infile);
    num_comms--;
  }

}


/*
void load_pcf_labels(char *trace_in) {

   FILE *pcf;
   char *pcf_name;


   pcf_name = strdup(trace_in);
   strcpy(pcf_name[strlen(pcf_name)-3], "pcf");


   if((pcf = fopen(pcf_name,"r")) == NULL) return;3


   while(fgets

}
*/


void
load_pcf (char *in_pcf)
{

  FILE *infile;
  unsigned long long id;
  char *word;
  int current_types[20];
  int index, fi_value, i, j;

  /* Open the files.  If NULL is returned there was an error */
  if ((infile = fopen (in_pcf, "r")) == NULL)
  {
    printf ("Can't open file %s\n", in_pcf);
    return;
  }


  next_type = 0;

  while (fgets (line, sizeof (line), infile) != NULL)
  {
    if (!strcmp (line, "EVENT_TYPE\n"))
    {
      index = 0;

      while (fgets (line, sizeof (line), infile) != NULL)
      {
        if (!strcmp (line, "VALUES\n"))
        {
          while (fgets (line, sizeof (line), infile) != NULL)
          {
            if (strstr (line, "DEFAULT")  ||
                strstr (line, "STATES")   ||
                strstr (line, "GRADIENT") ||
               !strcmp (line, "EVENT_TYPE\n"))
              break;


            if (strcmp (line, "\n") && !strstr (line, "#"))
            {
              word = strtok (line, " \t\n");
              id = atoll (word);

              word = strtok (NULL, "\n");

              if (word[strlen (word) - 1] == '\n')
                word[strlen (word) - 1] = '\0';

              for (i = 0; i < index; i++)
              {
                types[current_types[i]].values[types[current_types[i]].
                                               next_value].id = id;
                types[current_types[i]].values[types[current_types[i]].
                                               next_value].name =
                                                 strdup (word);
                types[current_types[i]].next_value++;
              }
            }
          }

          if (strstr (line, "DEFAULT") ||
              strstr (line, "STATES")  ||
              strstr (line, "GRADIENT"))
          {
            break;
          }

          if (!strcmp (line, "EVENT_TYPE\n"))
          {
            index = 0;
          }
        }

        if (strstr (line, "DEFAULT") ||
            strstr (line, "STATES")  ||
            strstr (line, "GRADIENT"))
        {
          break;
        }

        if (!strcmp (line, "EVENT_TYPE\n"))
          index = 0;

        if (strcmp (line, "\n")           &&
            strcmp (line, "EVENT_TYPE\n") &&
           !strstr (line, "#"))
        {
          word = strtok (line, " \t");
          word = strtok (NULL, " \t");
          types[next_type].id = atoll (word);

          word = strtok (NULL, "\n");

          if (word[strlen (word) - 1] == '\n')
          {
            word[strlen (word) - 1] = '\0';
          }

          types[next_type].name = strdup (word);
          types[next_type].next_value = 0;
          current_types[index] = next_type;

          index++;
          next_type++;
        }
      }
    }
  }
}

int
register_thread (int appl, int task, int thread)
{
  int i;

  threads[next_thread_slot].appl         = appl;
  threads[next_thread_slot].task         = task;
  threads[next_thread_slot].thread       = thread;
  threads[next_thread_slot].running_time = 0;
  threads[next_thread_slot].total_time   = 0;


  threads[next_thread_slot].cpus         = NULL;
  threads[next_thread_slot].calls        = NULL;
  threads[next_thread_slot].top          = -1;
  threads[next_thread_slot].comm_counter = NULL;

  i = next_thread_slot;
  next_thread_slot++;
  thread_pointer[appl][task][thread] = i;

  return i;
}


void
write_cpu_stats ()
{
  int i;

  fprintf (outfile, "CPU STATS\n\n");

  for (i = 0; i < MAX_CPUS; i++)
  {
    if (cpus[i] > 0)
    {
      fprintf (outfile,
               "\tCPU %d  Ocuppation %2.2f%%\n",
               i + 1,
               ((double) cpus[i] / (double) trace_time) * 100);
    }
  }

  fprintf (outfile, "\n\n");
}


void
inc_cpu_stats (int                thread_id,
               int                cpu,
               unsigned long long time_ini,
               unsigned long long time_end)
{

  struct cpu_info *cpu_index, *last_cpu;

  /* Searching for CPU info */
  cpu_index = threads[thread_id].cpus;

  while (cpu_index != NULL)
  {
    if (cpu_index->id == cpu)
    {
      cpu_index->time += time_end - time_ini;
      break;
    }

    last_cpu  = cpu_index;
    cpu_index = cpu_index->next_cpu;
  }

  /* The cpu isn't registered yet */
  if (cpu_index == NULL)
  {
    if ((cpu_index =
           (struct cpu_info *) malloc (sizeof (struct cpu_info))) == NULL)
    {
      printf ("ERROR: No more memory for cpu stats\n");
      exit (1);
    }


    cpu_index->id       = cpu;
    cpu_index->time     = time_end - time_ini;
    cpu_index->next_cpu = NULL;


    if (threads[thread_id].cpus == NULL)
    { /* First of the cpus */
      threads[thread_id].cpus = cpu_index;
    }
    else
    { /* Link new counter to list */
      last_cpu->next_cpu = cpu_index;
    }
  }
}


void
write_thread_cpu_stats ()
{
  int              i;
  struct cpu_info *cpu;


  fprintf (outfile, "\nCPU STATS BY THREAD");

  for (i = 0; i < next_thread_slot; i++)
  {
    fprintf (outfile,
             "\n\nThread %d:%d:%d   running %lld ns\n",
             threads[i].appl,
             threads[i].task,
             threads[i].thread,
             threads[i].running_time);

    for (cpu = threads[i].cpus; cpu != NULL; cpu = cpu->next_cpu)
    {
      fprintf (outfile,
               "    CPU %d:    %2.2f%%\n",
               cpu->id,
               ((double) cpu->time / (double) threads[i].running_time) * 100);
    }
  }
}

int
allowed_type (unsigned long long id)
{
  int i;

  if (next_allowed_type == 0)
  {
    return 1;
  }


  for (i = 0; i < next_allowed_type; i++)
  {
    if (allowed_types[i].ranged_type)
    {
      if (id <= allowed_types[i].max_type && id >= allowed_types[i].min_type)
      {
        return 1;
      }
    }
    else if (id == allowed_types[i].min_type)
    {
      return 1;
    }
  }

  return 0;
}



void
update_call_stats_exclusive (int                thread_id,
                             unsigned long long type,
                             unsigned long long value,
                             unsigned long long time)
{

  struct call_info *call, *last_call, *in_current_call;
  int i;

  /* Find the last call made */
  in_current_call = NULL;

  call            = threads[thread_id].calls;
  while (call != NULL)
  {
    if (call->type == type && call->in_call)
    {
      in_current_call = call;
      break;
    }

    call = call->next_call;
  }

  /* Update stats of the last call made */
  if (in_current_call != NULL)
  {
    in_current_call->time += time - in_current_call->ini_time;
    in_current_call->in_call = 0;
  }


  /* Find the current call made */
  call = threads[thread_id].calls;
  while (call != NULL)
  {
    if (call->type == type && call->value == value)
    {
      call->num++;
      call->ini_time = time;
      call->in_call  = 1;
      break;
    }

    last_call = call;
    call      = call->next_call;
  }

  if (call == NULL)
  {
    if ((call =
           (struct call_info *) malloc (sizeof (struct call_info))) == NULL)
    {
      printf ("ERROR: No more memory for call stats\n");
      exit (1);
    }

    call->type      = type;
    call->value     = value;
    call->ini_time  = time;
    call->time      = 0;
    call->num       = 1;
    call->in_call   = 1;
    call->next_call = NULL;

    if (threads[thread_id].calls == NULL)
    { /* First of the calls */
      threads[thread_id].calls = call;
    }
    else
    { /* Link new call to list */
      last_call->next_call = call;
    }
  }
}

void
update_call_stats (int                thread_id,
                   unsigned long long type,
                   unsigned long long value,
                   unsigned long long time)
{
  struct call_info *call, *last_call;
  int               i;

  if (value > 0)
  {
    call = threads[thread_id].calls;
    while (call != NULL)
    {
      if (call->type == type && call->value == value)
      {
        call->num++;
        call->ini_time = time;
        break;
      }

      last_call = call;
      call = call->next_call;
    }

    if (call == NULL)
    {
      if ((call =
             (struct call_info *) malloc (sizeof (struct call_info))) == NULL)
      {
        printf ("ERROR: No more memory for call stats\n");
        exit (1);
      }

      call->type      = type;
      call->value     = value;
      call->ini_time  = time;
      call->time      = 0;
      call->num       = 1;
      call->next_call = NULL;

      if (threads[thread_id].calls == NULL)
      { /* First of the calls */
        threads[thread_id].calls = call;
      }
      else
      { /* Link new call to list */
        last_call->next_call = call;
      }
    }

    threads[thread_id].top++;
    threads[thread_id].stack[threads[thread_id].top] = call;
  }
  else
  {
    for (i = threads[thread_id].top; i >= 0; i--)
    {
      if (threads[thread_id].stack[i]->type == type)
      {
        threads[thread_id].stack[i]->time +=
          time - threads[thread_id].stack[i]->ini_time;

        threads[thread_id].top = i - 1;
        break;
      }
    }

    if (i < 0)
    {
      call = threads[thread_id].calls;
      while (call != NULL)
      {
        if (call->type == type && call->value == value)
        {
          call->num++;
          break;
        }

        last_call = call;
        call      = call->next_call;
      }

      if (call == NULL)
      {
        if ((call =
               (struct call_info *) malloc (sizeof (struct call_info))) == NULL)
        {
          printf ("ERROR: No more memory for call stats\n");
          exit (1);
        }

        call->type      = type;
        call->value     = value;
        call->time      = 0;
        call->num       = 1;
        call->next_call = NULL;
      }

      if (threads[thread_id].calls == NULL)
      { /* First of the calls */
        threads[thread_id].calls = call;
      }
      else
      { /* Link new call to list */
        last_call->next_call = call;
      }
    }
  }
}



void
write_thread_call_stats ()
{
  int  i, j, k, z, call_printed;
  char type_label[256], value_label[256];

  struct call_info  *call;
  unsigned long long total_call_time;


  if ((outfile = fopen (logfile, "w")) == NULL)
  {
    printf ("Error Opening Logfile File %s\n", logfile);
    exit (1);
  }

  fprintf (outfile, "\n\nCALL STATS BY THREAD");

  for (i = 0; i < next_thread_slot; i++)
  {
    fprintf (outfile, "\n\nThread %d:%d:%d", threads[i].appl, threads[i].task,
             threads[i].thread);


    call_printed = 0;
    total_call_time = 0;

    for (call = threads[i].calls; call != NULL; call = call->next_call)
    {
      /* Searching of the labels for that type and value */
      for (k = 0; k < next_type; k++)
      {
        if (types[k].id == call->type)
        {
          strcpy (type_label, types[k].name);

          for (z = 0; z < types[k].next_value; z++)
          {
            if (types[k].values[z].id == call->value)
            {
              strcpy (value_label, types[k].values[z].name);
              break;
            }
          }

          if (z == types[k].next_value)
          {
            sprintf (value_label, "    %lld", call->value);
          }
          break;
        }
      }

      if (k == next_type)
      {
        sprintf (type_label, "    %lld", call->type);
        sprintf (value_label, "    %lld", call->value);
      }

      /* Printing of the results */
      if (call->time > 0)
      {
        total_call_time += call->time;
        fprintf (outfile,
                 "\n    Type: %s   Value: %s   Num of calls: %lld  Total time: %lld   %% over the total trace time: %2.2f%%\n",
                 type_label,
                 value_label,
                 call->num,
                 call->time,
                 ((double) call->time / (double) trace_time) * 100);

        /*  Old version calculating % respect thread timme
        if(threads[i].total_time!=0)
          fprintf(outfile,"\n    Type: %s   Value: %s   Num of calls: %lld\n\t Total time: %lld   %% over the total trace time: %2.2f%%\n",type_label,value_label,call->num,call->time,((double)call->time/(double)threads[i].total_time)*100);
        else
          fprintf(outfile,"\n    Type: %s   Value: %s   Num of calls: %lld\n\t Total time: %lld\n",type_label,value_label,call->num,call->time);
        */
        call_printed = 1;
      }
      else
      {
        fprintf (outfile,
                 "\n    Type: %s   Value: %s    Num of calls: %lld\n",
                 type_label,
                 value_label,
                 call->num);
        call_printed = 1;
      }
    }

    if (!call_printed)
    {
      fprintf (outfile, "\n    No calls!!\n");
    }

    /* Printing total call time */
    fprintf (outfile,
             "\n\n    Total call time: %lld   %% over the total trace time: %2.2f%%\n\n",
             total_call_time,
             ((double) total_call_time / (double) trace_time) * 100);

  }
}


void
inc_comm_counter (int thread_id, int task_rcv, int size)
{
  int               i;
  struct comm_info *comm, *last_comm;


  comm = threads[thread_id].comm_counter;
  while (comm != NULL)
  {
    if (comm->task_rcv == task_rcv && comm->size == size)
    {
      comm->num_comm++;
      break;
    }

    last_comm = comm;

    comm = comm->next_comm;
  }


  if (comm == NULL)
  {
    if ((comm =
           (struct comm_info *) malloc (sizeof (struct comm_info))) == NULL)
    {
      printf ("ERROR: No more memory for counters\n");
      exit (1);
    }

    comm->task_rcv  = task_rcv;
    comm->size      = size;
    comm->num_comm  = 1;
    comm->next_comm = NULL;


    if (threads[thread_id].comm_counter == NULL)
    { /* First of the counters */
      threads[thread_id].comm_counter = comm;
    }
    else
    { /* Link new counter to list */
      last_comm->next_comm = comm;
    }
  }
}


void
write_comm_stats ()
{
  int               i;
  struct comm_info *comm;


  fprintf (outfile, "\nCOMMUNICATION STATS");

  for (i = 0; i < next_thread_slot; i++)
  {
    fprintf (outfile,
             "\n\nSender %d:%d:%d\n",
             threads[i].appl,
             threads[i].task,
             threads[i].thread);

    if (threads[i].comm_counter == NULL)
    {
      fprintf (outfile, "   NO COMMS !!!\n");
    }
    else
    {
      for (comm = threads[i].comm_counter; comm != NULL;
           comm = comm->next_comm)
      {
        fprintf (outfile,
                 "  Task Receiver: %d\tSize: %d \tNum_comms: %d\n",
                 comm->task_rcv,
                 comm->size,
                 comm->num_comm);
      }
    }
  }
}


void
ini_progress_bar (char *file_name)
{

  struct stat file_info;
  if (stat (file_name, &file_info) < 0)
  {
    perror ("Error calling stat");
    exit (1);
  }

  total_trace_size = file_info.st_size;


  /* Depen mida traça mostrem percentatge amb un interval diferent de temps
  if (total_trace_size < 500000000)
    total_iters = 500000;
  else
    total_iters = 5000000;
  */

  printf("\r%s 00%%", "Processing trace");
  fflush(stdout);

  last_showed = 0;
}


void
show_progress_bar (unsigned long line)
{
  double current_showed;

  current_readed_size = ftell (infile);
  current_showed      =
    ceil(((double)current_readed_size/(double)total_trace_size)*100);

  if (last_showed != current_showed)
  {
    printf("\r%s %2.0f%%", "Processing trace", current_showed);
    // printf("\r%s %2.0f%%", "Processing trace", line/1.0);
    fflush(stdout);
    last_showed = current_showed;
  }

  fflush(stdout);
}

void
end_progress_bar()
{
  printf("\r%s %2.0f%%", "Processing trace", 100.0);
  fflush(stdout);
}

void
write_histos ()
{
  FILE *fp;
  char  histo_bursts_file[1024], histo_comms_file[1024], histo_events_file[1024];
  int   i;

  /* Generating data burst file */
  if (show_bursts_histo)
  {
    strcpy (histo_bursts_file, new_file_name);
    if (!only_dat_file)
    {
      char *c = strrchr (histo_bursts_file, '.');
      strcpy (c, ".bursts.dat");
    }

    if ((fp = fopen (histo_bursts_file, "w")) == NULL)
    {
      printf ("Error: can't create file %s\n", histo_bursts_file);
      exit (1);
    }

    printf ("\nGenerating file %s", histo_bursts_file);

    /* Dumping bursts histo */
    for (i = 0; i < MAX_BURST_CLASS; i++)
    {
      fprintf (fp,
               "%lld\t%2.2f\n",
               bursts_histo.n_burst_class[i],
               (((double) bursts_histo.burst_class[i] /
                 (double) bursts_histo.total_run_time) * 100));
    }

    fclose (fp);

    /* writing GNUPLOT file */
    if (!only_dat_file)
    {
      char *gnuplot_file = (char *) malloc (strlen (new_file_name) + 12);
      strcpy (gnuplot_file, new_file_name);
      char *c = strrchr (gnuplot_file, '.');
      strcpy (c, ".bursts.gnuplot");

      if ((fp = fopen (gnuplot_file, "w")) == NULL)
      {
        printf ("Error: can't create file %s\n", gnuplot_file);
        exit (1);
      }

      printf ("\nGenerating file %s", gnuplot_file);

      fprintf (fp, "set title \"Histogram of bursts\"\n");
      fprintf (fp, "set key top left\n");
      fprintf (fp, "set ylabel \"Num. bursts\"\n");
      fprintf (fp, "set y2label \"%% of running time\"\n");
      fprintf (fp, "set ytics nomirror\n");
      fprintf (fp, "set y2tics\n");
      fprintf (fp, "set y2range [1:100]\n");
      fprintf (fp,
               "set xtics (\"1 us\" 0, \"10 us\" 2, \"100 us\" 4, \"1 ms\" 6, \"10 ms\" 8, \"100 ms\" 10, \"1s\" 12, \"10 s\" 14)\n");
      fprintf (fp, "set offsets 2, 2, 0, 0\n");
      fprintf (fp, "set style line 2 linecolor rgb \"#32CD32\"\n");
      fprintf (fp, "set boxwidth 0.5 absolute\n");
      fprintf (fp, "set style fill  solid 1.00 border -1\n");
      fprintf (fp,
               "plot \'%s\' using 0:1 t \"num burst\" w lines, \'%s\' using 0:2 axes x1y2 ls 2 t \"%% of time\" with boxes\n",
               histo_bursts_file, histo_bursts_file);
      fprintf (fp, "pause -1 \"Press return to continue...\"");


      fclose (fp);

      free (gnuplot_file);
    }
  }

  /* Generating data comms file */
  if (show_comms_histo)
  {
    strcpy (histo_comms_file, new_file_name);
    if (!only_dat_file)
    {
      char *c = strrchr (histo_comms_file, '.');
      strcpy (c, ".comms.dat");
    }

    if ((fp = fopen (histo_comms_file, "w")) == NULL)
    {
      printf ("Error: can't create file %s\n", histo_comms_file);
      exit (1);
    }

    printf ("\nGenerating file %s", histo_comms_file);

    /* Dumping comms histo */
    for (i = 0; i < MAX_COMMS_CLASS; i++)
    {
      fprintf (fp,
               "%lld\t%2.2f\n",
               comms_histo.n_comms_class[i],
               (((double) comms_histo.comms_class[i] /
                 (double) comms_histo.total_bytes_send) * 100));
    }

    fclose (fp);

    /* Generating .gnuplot file */
    if (!only_dat_file)
    {
      char *gnuplot_file = (char *) malloc (strlen (new_file_name) + 11);
      strcpy (gnuplot_file, new_file_name);
      char *c = strrchr (gnuplot_file, '.');
      strcpy (c, ".comms.gnuplot");

      if ((fp = fopen (gnuplot_file, "w")) == NULL)
      {
        printf ("Error: can't create file %s\n", gnuplot_file);
        exit (1);
      }

      printf ("\nGenerating file %s", gnuplot_file);

      fprintf (fp, "set title \"Histogram of Communications\"\n");
      fprintf (fp, "set key top left\n");
      fprintf (fp, "set ylabel \"Num. comms\"\n");
      fprintf (fp, "set y2label \"%% of bytes sent\"\n");
      fprintf (fp, "set ytics nomirror\n");
      fprintf (fp, "set y2tics\n");
      fprintf (fp, "set y2range [1:100]\n");
      fprintf (fp,
               "set xtics (\"10 B\" 0, \"100 B\" 1, \"1 KB\" 2, \"10 KB\" 3, \"100 KB\" 4, \"1 MB\" 5, \"10 MB\" 6, \"> 10 MB\" 7)\n");
      fprintf (fp, "set offsets 2, 2, 0, 0\n");
      fprintf (fp, "set style line 2 linecolor rgb \"#32CD32\"\n");
      fprintf (fp, "set boxwidth 0.5 absolute\n");
      fprintf (fp, "set style fill  solid 1.00 border -1\n");
      fprintf (fp,
               "plot \'%s\' using 0:1 t \"num comms\" w lines, \'%s\' using 0:2 axes x1y2 ls 2 t \"%% of bytes sent\" with boxes\n",
               histo_comms_file, histo_comms_file);
      fprintf (fp, "pause -1 \"Press return to continue...\"");

      fclose (fp);
      free (gnuplot_file);
    }
  }

  if (show_events_histo)
  {
    strcpy (histo_events_file, new_file_name);

    if (!only_dat_file)
    {
      char *c = strrchr (histo_events_file, '.');
      strcpy (c, ".events.dat");
    }


    if ((fp = fopen (histo_events_file, "w")) == NULL)
    {
      printf ("Error: can't create file %s\n", histo_events_file);
      exit (1);
    }

    printf ("\nGenerating file %s", histo_events_file);

    /* Dumping events histo */
    for (i = 0; i < events_histo.last_free_slot; i++)
    {
      fprintf (fp, "%lld\t", events_histo.counters[i]);
    }

    fclose (fp);

    /* Generating .gnuplot file */
    if (!only_dat_file)
    {
      char *gnuplot_file = (char *) malloc (strlen (new_file_name) + 12);
      strcpy (gnuplot_file, new_file_name);
      char *c = strrchr (gnuplot_file, '.');
      strcpy (c, ".events.gnuplot");

      if ((fp = fopen (gnuplot_file, "w")) == NULL)
      {
        printf ("Error: can't create file %s\n", gnuplot_file);
        exit (1);
      }

      printf ("\nGenerating file %s", gnuplot_file);

      fprintf (fp, "set title \"Histogram of Events\"\n");
      fprintf (fp, "set key top right\n");
      fprintf (fp, "set ylabel \"Num. calls\"\n");
      fprintf (fp, "set yrange [0:*]\n");
      fprintf (fp, "set boxwidth 0.9 absolute\n");
      fprintf (fp, "set style fill  solid 1.00 border -1\n");
      fprintf (fp,
               "set style histogram clustered gap 1 title  offset character 0, 0, 0\n");
      fprintf (fp, "set datafile missing \'-\'\n");
      fprintf (fp, "set noxtics\n");
      fprintf (fp, "set style data histograms\n");
      fprintf (fp, "plot \'%s\' using 1 t \"%lld\"", histo_events_file,
               events_histo.types[0]);

      for (i = 1; i < events_histo.last_free_slot; i++)
      {
        fprintf (fp,
                 ", \'\' using %d t \"%lld\"",
                 i + 1,
                 events_histo.types[i]);
      }
      fprintf (fp, "\npause -1 \"Press return to continue...\"");

      fclose (fp);
      free (gnuplot_file);
    }
  }
}


int
main (int argc, char *argv[])
{
  int Matches;
  int id, cpu, appl, task, thread, state, task_rcv, i, j, k, end_line;
  unsigned long long time_1, time_2, call_time, type, value, burst_time, size;
  char *word;
  unsigned long num_iters = 0;


  /* Ini data */
  next_thread_slot = 0;


  for (i = 0; i < 1; i++)
    for (j = 0; j < 12000; j++)
      for (k = 0; k < 10; k++)
        thread_pointer[i][j][k] = -1;


  for (i = 0; i < MAX_CPUS; i++)
    cpus[i] = 0;


  /* Ini burst histo */
  bursts_histo.total_run_time = 0;
  for (i = 0; i < MAX_BURST_CLASS; i++)
  {
    bursts_histo.burst_class[i] = 0;
    bursts_histo.n_burst_class[i] = 0;
  }


  /* Ini comms histo */
  comms_histo.total_bytes_send = 0;
  for (i = 0; i < MAX_COMMS_CLASS; i++)
  {
    comms_histo.comms_class[i] = 0;
    comms_histo.n_comms_class[i] = 0;
  }

  events_histo.last_free_slot = 0;


  /* Reading of the program arguments */
  read_args (argc, argv);


  /* Open the files.  If NULL is returned there was an error */
  if ((infile = fopen (argv[1], "r")) == NULL)
  {
    printf ("Error Opening Input File %s\n", argv[1]);
    exit (1);
  }

  ini_progress_bar (argv[1]);

  /* Substitution of .prv to .pcf, and loading of pcf labels */
  /*  word = strrchr(argv[1],'.');
    *word = '\0';
    strcat(argv[1],".pcf");
    load_pcf(argv[1]);
  */

  // printf ("\nReading trace and generating statistics...");
  // fflush (stdout);


  /* Process header */
  char *trace_header;
  trace_header = (char *) malloc (sizeof (char) * MAX_HEADER_SIZE);
  fgets (trace_header, MAX_HEADER_SIZE, infile);
  proces_header (trace_header);
  free (trace_header);


  unsigned long current_line = 1;

  /* Processing the trace records */
  while ((Matches = 
            fscanf (infile,
                    "%d:%d:%d:%d:%d:%lld:",
                    &id,
                    &cpu,
                    &appl,
                    &task,
                    &thread,
                    &time_1)) != EOF)
  {

    if (Matches != 6)
    { /* Should be a comment */
      fgets (line, sizeof (line), infile);
      continue;
    }
    
    show_progress_bar(current_line);
    current_line++;
    
    /* 1: state; 2: event; 3: comm; 4: global comm */
    switch (id)
    {
      case 1:
        fscanf (infile, "%lld:%d\n", &time_2, &state);

        /* Stats for the cpu */
        if (show_cpus && cpu > 0)
          cpus[cpu - 1] += time_2 - time_1;

        /* Burst histogram */
        if (show_bursts_histo && state == 1)
        {
          burst_time = time_2 - time_1;
          bursts_histo.total_run_time += burst_time;

          if (burst_time <= 1000)
          {
            i = 0;
          }
          else if (burst_time <= 5000)
          {
            i = 1;
          }
          else if (burst_time <= 10000)
          {
            i = 2;
          }
          else if (burst_time <= 50000)
          {
                  i = 3;
          }
          else if (burst_time <= 100000)
          {
            i = 4;
          }
          else if (burst_time <= 500000)
          {
            i = 5;
          }
          else if (burst_time <= 1000000)
          {
            i = 6;
          }
          else if (burst_time <= 5000000)
          {
            i = 7;
          }
          else if (burst_time <= 10000000)
          {
            i = 8;
          }
          else if (burst_time <= 50000000)
          {
            i = 9;
          }
          else if (burst_time <= 100000000)
          {
            i = 10;
          }
          else if (burst_time <= 500000000)
          {
            i = 11;
          }
          else if (burst_time <= 1000000000)
          {
            i = 12;
          }
          else if (burst_time <= 5000000000)
          {
            i = 13;
          }
          else if (burst_time <= 10000000000)
          {
            i = 14;
          }
          else
          {
            i = 15;
          }

          bursts_histo.burst_class[i] += burst_time;
          bursts_histo.n_burst_class[i]++;
        }

        /* If not allowed thread, next event */
        if (rank_threads && (task < rank_min || task > rank_max))
        {
          break;
        }

        /* If we don't have the thread registered yet */
        if ((id = thread_pointer[appl - 1][task - 1][thread - 1]) == -1)
        {
          id = register_thread (appl - 1, task - 1, thread - 1);
        }

        threads[id].total_time += time_2 - time_1;

        if (cpu > 0)
        {
          threads[id].running_time += time_2 - time_1;
        }

        /* If we don't have to compute stats, next event */
        if (cpu == 0)
        {
          break;
        }

        if (!show_thread_resources)
        {
          break;
        }

        inc_cpu_stats (id, cpu, time_1, time_2);
        break;

      case 2:
        fgets (line, sizeof (line), infile);

        /* If we don't have to show calls */
        if (!show_thread_calls && !show_events_histo)
          break;

        /* If not allowed thread, next event */
        if (rank_threads && (task < rank_min || task > rank_max))
          break;

        /* If we don't have the thread registered yet */
        if ((id = thread_pointer[appl - 1][task - 1][thread - 1]) == -1)
          id = register_thread (appl - 1, task - 1, thread - 1);

        /* Event type and values */
        end_line = 0;

        word = strtok (line, ":");

        type = atoll (word);

        word = strtok (NULL, ":");

        value = atoll (word);

        while (!end_line)
        {

          if (show_events_histo && allowed_type (type))
          {

            for (i = 0; i < events_histo.last_free_slot; i++)
            {
              if (events_histo.types[i] == type)
              {
                if (value > 0)
                  events_histo.counters[i]++;

                break;
              }
            }

            if (i == events_histo.last_free_slot)
            {
              events_histo.types[i] = type;
              events_histo.counters[i] = 1;
              events_histo.last_free_slot++;

              if (events_histo.last_free_slot == MAX_HISTO_EVENTS)
              {
                printf ("NO MORE MEMORY for events histo!!\n");
                exit (1);
              }
            }
          }


          if (show_thread_calls)
          {

            if (allowed_type (type))
            {
              if (!calls_exclusive_time)
                update_call_stats (id, type, value, time_1);
              else
                update_call_stats_exclusive (id, type, value, time_1);

            }
          }

          if ((word = strtok (NULL, ":")) != NULL)
          {
            type = atoll (word);
            word = strtok (NULL, ":");
            value = atoll (word);
          }
          else
            end_line = 1;


        }

        break;

      case 3:
        fscanf (infile, "%*lld:%*d:%*d:%d:%*d:%*lld:%*lld:%lld:%*d\n",
                &task_rcv, &size);

        if (show_comms_histo)
        {

          comms_histo.total_bytes_send += size;

          if (size <= 10)
            i = 0;
          else
            if (size <= 100)
              i = 1;
            else
              if (size <= 1024)
                i = 2;
              else
                if (size <= 10240)
                  i = 3;
                else
                  if (size <= 102400)
                    i = 4;
                  else
                    if (size <= 1048576)
                      i = 5;
                    else
                      if (size <= 10485760)
                        i = 6;
                      else
                        i = 7;

          comms_histo.comms_class[i] += size;

          comms_histo.n_comms_class[i]++;
        }


        /* If we don't have to show calls */
        if (!show_comms)
          break;

        /* If not allowed thread, next event */
        if (rank_threads && (task < rank_min || task > rank_max))
          break;

        /* If we don't have the thread registered yet */
        if ((id = thread_pointer[appl - 1][task - 1][thread - 1]) == -1)
          id = register_thread (appl - 1, task - 1, thread - 1);

        inc_comm_counter (id, task_rcv, size);
        break;

      case 4:
        fgets (line, sizeof (line), infile);

        break;
    }


  }

  end_progress_bar();

  // printf ("Done\n\n");


  /* Dumping the stats into a file */
  /*  printf("Writing CPU stats...\n");
    if(show_cpus) write_cpu_stats();
    printf("Writing thread stats...\n");
    if(show_thread_resources) write_thread_cpu_stats();
  */

  if (show_thread_calls)
  {
    printf ("\nGenerating call statistic in log %s", logfile);
    write_thread_call_stats ();
  }

  /*  if(show_comms) write_comm_stats();
  */
  if (show_bursts_histo || show_comms_histo || show_events_histo)
    write_histos ();

  printf ("\n\n");

  /* Close the files */
  fclose (infile);
}
