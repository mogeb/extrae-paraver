#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <search.h>
#include <ctype.h>

#include "call_renaming.h"
#include "hashtab.h"

#define MAX_HEADER_SIZE 1048576
#define MAX_LABELS         1000
#define MAX_LINE          32768

/* Execution parameters */


/* Trace in and trace out */
FILE *infile, *outfile;


/* Structs */
struct call_label trace_labels_1[ MAX_LABELS ];
int next_label_slot_1 = 0;

struct call_label trace_labels_2[ MAX_LABELS ];
int next_label_slot_2 = 0;

hashtab_t *hash_ids;


/* Parameter for showing the percentage */
unsigned long long total_trace_size, current_readed_size;
double last_showed = 0;
unsigned long total_iters = 0;


void parse_args( int argc, char *argv[] )
{
  if ( argc != 4 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help" ))
  {
    printf("\n\nUsage:\n\t%s <reference_trace> <trace_to_translate> <generated_trace>\n\n",argv[0]);
    exit(1);
  }
}


/* Loading id's from the original trace */
void load_pcf_ids( char *pcf_name, struct call_label call_ids[], int *next_slot )
{
  FILE *pcf;
  char *word, *word_aux, *c;
  int current_type, next_value, i;
  char line[MAX_LINE];

  if (( pcf = fopen(pcf_name,"r")) == NULL )
  {
    printf("Error Opening .pcf %s\n\n", pcf_name);
    exit(1);
  }

  while(fgets(line, sizeof(line), pcf)!=NULL)
  {
    if(strstr(line, "EVENT_TYPE")!=NULL)
    {
      current_type = *next_slot;
      while(fgets(line, sizeof(line), pcf)!=NULL)
      {
        if(isdigit(line[0]))
        {
          word = strtok(line, " \t\n");
          call_ids[*next_slot].colour = atoll(word);

          word = strtok(NULL, " \t\n");
          call_ids[*next_slot].type = atoll(word);

          word = strtok(NULL, "\n");
          /* Removing white spaces in name */
          c = word;
          while(*c == ' ')
            c++;

          word = c;

          c = &word[strlen(word)];
          while(*c == ' ')
            c--;

          *c = '\0';

          call_ids[*next_slot].name = strdup(word);

          (*next_slot)++;
          continue;
        }

        if(strstr(line, "VALUES")!=NULL)
        {
          next_value = 0;
          while(fgets(line, sizeof(line), pcf)!=NULL)
          {
            if(isdigit(line[0]))
            {
              word = strtok(line, "  ");
              word_aux = strtok(NULL, "\n");

              /* Removing white spaces in name */
              c = word_aux;
              while(*c == ' ')
                c++;

              word_aux = c;

              c = &word_aux[strlen(word_aux)];
              while(*c == ' ')
                c--;

              *c = '\0';

              for(i = current_type; i < *next_slot; i++)
              {
                call_ids[i].value_ids[next_value] = atoll(word);
                call_ids[i].value_name[next_value] = strdup(word_aux);
              }
              next_value++;

              continue;
            }
// AQUI HI HA UN BUG SI NO HI HA RES MES EN EL FITXER DESPRES DE L'ULTIM VALOR
            for(i = current_type; i < *next_slot; i++)
              call_ids[i].next_value_slot = next_value;

            break;
          }
        }

        break;
      }
    }
  }

  fclose(pcf);
}


void recalculate_ids()
{
  int i, j, z, k, collision;
  unsigned long long free_id, new_value;
  char key[512], value_c[512];

  for(i = 0; i < next_label_slot_2; i++)
  {
    /* First search this type in the old pcf */
    for(k = 0; k < next_label_slot_1; k++)
    {
      if(trace_labels_1[k].type == trace_labels_2[i].type)
      {
        /* For every value, recalculate it if needed */

        for(j = 0; j < trace_labels_2[i].next_value_slot; j++)
        {
          collision = 0;
          free_id = 0;
          new_value = trace_labels_2[i].value_ids[j];
          for(z = 0; z < trace_labels_1[k].next_value_slot; z++)
          {
            if(!strcmp(trace_labels_1[k].value_name[z], trace_labels_2[i].value_name[j]))
            {
              new_value = trace_labels_1[k].value_ids[z];
              collision = 0;
              break;
            }
            else if(trace_labels_1[k].value_ids[z] == trace_labels_2[i].value_ids[j])
              collision = 1;

            if(trace_labels_1[k].value_ids[z] > free_id)
              free_id = trace_labels_1[k].value_ids[z];
          }

          /* Si tenim colision, renombrem id */
          if(collision)
          {
            new_value = ++free_id;

            /* Afegim aquest id als del pcf antic */
            trace_labels_1[k].value_ids[trace_labels_1[k].next_value_slot] = new_value;
            trace_labels_1[k].value_name[trace_labels_1[k].next_value_slot] = strdup(trace_labels_2[i].value_name[j]);
            trace_labels_1[k].next_value_slot++;
          }

          /* Insertem a hash */
          sprintf(key, "%lld:%lld", trace_labels_2[i].type, trace_labels_2[i].value_ids[j]);
          sprintf(value_c, "%lld", new_value);
          if(ht_insert(hash_ids, (char *)key, strlen(key), (char *)value_c, strlen(value_c))== NULL)
          {
            printf("Error inserting at hash table\n");
            exit(1);
          }
        }

        break;
      }
    }

    /* If there is a type in pcf2 but not in pcf1 */
    if(k == next_label_slot_1)
    {
      trace_labels_1[k].type = trace_labels_2[i].type;
      trace_labels_1[k].name = strdup(trace_labels_2[i].name);
      trace_labels_1[k].colour = trace_labels_2[i].colour;

      for(z = 0; z < trace_labels_2[i].next_value_slot; z++)
      {
        trace_labels_1[k].value_ids[z] = trace_labels_2[i].value_ids[z];
        trace_labels_1[k].value_name[z] = strdup(trace_labels_2[i].value_name[z]);
      }
      trace_labels_1[k].next_value_slot = trace_labels_2[i].next_value_slot;
      next_label_slot_1++;
    }
  }
}


/* For processing the Paraver header */
unsigned long long proces_header(char *header, FILE *infile, FILE *outfile)
{
  int num_comms;
  char *word;
  unsigned long long readedBytes = 0;

  /* Obtaining the total trace time */
  readedBytes = strlen(header);
  word = strtok(header,":");
  word = strtok(NULL, ":");
  word = strtok(NULL, ":");
  word = strtok(NULL,"\n");
  word = strrchr(word,',');

  /* Obtaining th number of communicators */
  strcpy(header, word+1);
  if(strchr(header,')')!=NULL) return 0;
  num_comms = atoi(header);

  while(num_comms > 0)
  {
    if (fgets(header, MAX_HEADER_SIZE, infile) == NULL)
    {
      perror ("fgets");
      exit (1);
    }
    readedBytes += strlen(header);
    fprintf(outfile,"%s",header);
    num_comms--;
  }

  return readedBytes;
}


void ini_progress_bar(char *trace_name)
{
  struct stat file_info;

  if(stat(trace_name, &file_info)<0)
  {
    perror("Error calling stat64");
    exit(1);
  }
  total_trace_size = file_info.st_size;

  /* Depen mida traça mostrem percentatge amb un interval diferent de temps */
  if(total_trace_size < 500000000)
    total_iters =  500000;
  else
    total_iters = 5000000;
}


void show_progress_bar()
{
  double current_showed;

  current_readed_size = (unsigned long long)ftello64(infile);
  current_showed = ceil(((double)current_readed_size/(double)total_trace_size)*100);

  if(last_showed != current_showed)
  {
    printf("...%2.0f%%", current_showed);
    last_showed = current_showed;
  }

  fflush(stdout);
}


void copy_file(char *in, char *out)
{
  FILE *fileIn, *fileOut;
  char line[MAX_LINE];

  if((fileIn = fopen(in,"r")) == NULL)
    return;

  fileOut = fopen(out,"w");

  while(fgets(line,sizeof(line),fileIn)!=NULL)
    fputs(line, fileOut);

  fclose(fileIn);
  fclose(fileOut);
}


void write_new_pcf(char *pcf_name_in, char *pcf_name_out)
{
  FILE *pcf_in, *pcf_out;
  char *word, *word_aux;
  unsigned long long type, value, new_value;
  int i, j, num_end_lines = 0;
  char line[MAX_LINE];

  /* Open files */
  if((pcf_in = fopen(pcf_name_in,"r")) == NULL)
  {
    printf("Error Opening File %s\n", pcf_name_in);
    exit(1);
  }

  if((pcf_out = fopen(pcf_name_out,"w")) == NULL)
  {
    printf("Error Opening File %s\n", pcf_name_out);
    exit(1);
  }

  /* Writting the pcf part withiout EVENTS and VALUES */
  while(fgets(line, sizeof(line), pcf_in)!=NULL)
  {
    if(strstr(line, "EVENT_TYPE")!=NULL)
    {
      while(fgets(line, sizeof(line), pcf_in)!=NULL)
      {
        if(isdigit(line[0]))
          continue;

        if(strstr(line, "VALUES")!=NULL)
        {
          while(fgets(line, sizeof(line), pcf_in)!=NULL)
          {
            if(isdigit(line[0]))
              continue;
            else
              break;
          }
        }

        /* In order to make a nice .pcf */
        if(isspace(line[0]))
        {
          if(num_end_lines < 3)
          {
            fputs(line, pcf_out);
            num_end_lines++;
          }
        }
        else
          num_end_lines = 0;

        break;
      }
    }
    else
    {
      /* In order to make a nice .pcf */
      if(isspace(line[0]))
      {
        if(num_end_lines < 3)
        {
          fputs(line, pcf_out);
          num_end_lines++;
        }
      }
      else
      {
        fputs(line, pcf_out);
        num_end_lines = 0;
      }
    }
  }

  /* Dumping all the EVENTS and VALUES */
  for(i = 0; i < next_label_slot_1; i ++)
  {
    fprintf(pcf_out, "EVENT_TYPE\n");
    fprintf(pcf_out, "%d   %lld   %s\n", trace_labels_1[i].colour, trace_labels_1[i].type, trace_labels_1[i].name);

    if(trace_labels_1[i].next_value_slot > 0)
    {
      fprintf(pcf_out, "VALUES\n");

      for(j = 0; j < trace_labels_1[i].next_value_slot; j++)
        fprintf(pcf_out, "%lld   %s\n", trace_labels_1[i].value_ids[j], trace_labels_1[i].value_name[j]);
    }

    fprintf(pcf_out, "\n\n\n");
  }

  fclose(pcf_in);
  fclose(pcf_out);
}


int main(int argc, char *argv[])
{
  int i, num_char, end_line;
  unsigned long long time_1, time_2, type, value, new_value;
  char *word, event_record[MAX_LINE], *trace_header, *trace_in_1, *trace_in_2;
  unsigned long num_iters = 0;
  char line[MAX_LINE];

  parse_args(argc, argv);

  /* Creating the hash table */
  hash_ids = ht_init(100000, NULL);

  /* Obrim el pcf per carregar simbols */
  printf("\nLoading pcf symbols...\n");
  char *pcf1_in;

  pcf1_in = strdup(argv[1]);
  pcf1_in[strlen(pcf1_in)-4] = '\0';
  sprintf(pcf1_in, "%s.pcf", pcf1_in);

  load_pcf_ids(pcf1_in, trace_labels_1, &next_label_slot_1);

  free(pcf1_in);

  /*
    int k, z;
    for(k = 0; k < next_label_slot_1; k++){
  	printf("\nTYPE %lld\n", trace_labels_1[k].type);

  	for(z = 0; z < trace_labels_1[k].next_value_slot; z++)
  		printf("\t%lld %s\n", trace_labels_1[k].value_ids[z], trace_labels_1[k].value_name[z]);
    }
  */

  char *pcf2_in;

  pcf2_in = strdup(argv[2]);
  pcf2_in[strlen(pcf2_in)-4] = '\0';
  sprintf(pcf2_in, "%s.pcf", pcf2_in);

  load_pcf_ids(pcf2_in, trace_labels_2, &next_label_slot_2);

  printf("\nRecalculating symbols...\n");

  recalculate_ids();
  /*
    for(k = 0; k < next_label_slot_1; k++){
          printf("\nTYPE %lld\n", trace_labels_1[k].type);

          for(z = 0; z < trace_labels_1[k].next_value_slot; z++)
                  printf("\t%lld %s\n", trace_labels_1[k].value_ids[z], trace_labels_1[k].value_name[z]);
    }
  */
  /* Generem el nou pcf */
  printf("\nGenerating new .pcf...\n");
  char *pcf3_in;

  pcf3_in = strdup(argv[3]);
  pcf3_in[strlen(pcf3_in)-4] = '\0';
  sprintf(pcf3_in, "%s.pcf", pcf3_in);

  write_new_pcf(pcf2_in, pcf3_in);

  free(pcf2_in);
  free(pcf3_in);

  /* Copiem el .row */
  char *rowIn, *rowOut;

  rowIn = strdup(argv[2]);
  rowOut = strdup(argv[3]);
  rowIn[strlen(rowIn)-4] = '\0';
  rowOut[strlen(rowOut)-4] = '\0';
  sprintf(rowIn, "%s.row", rowIn);
  sprintf(rowOut, "%s.row", rowOut);

  copy_file(rowIn, rowOut);

  free(rowIn);
  free(rowOut);

  printf("\nRenaming call ids in trace...");
  fflush(stdout);

  /* Open files */
  if((infile = fopen(argv[2],"r")) == NULL)
  {
    printf("Error Opening File %s\n", argv[2]);
    exit(1);
  }

  if((outfile = fopen(argv[3],"w")) == NULL)
  {
    printf("Error Opening File %s\n", argv[3]);
    exit(1);
  }

  ini_progress_bar(argv[2]);

  /* Process header */
  trace_header = (char *)malloc(sizeof(char)*MAX_HEADER_SIZE);
  if (fgets(trace_header, MAX_HEADER_SIZE, infile) == NULL)
  {
    perror ("fgets");
    exit (1);
  }

  fprintf(outfile,"%s",trace_header);

  proces_header(trace_header, infile, outfile);

  free(trace_header);

  /* Processing the trace records */
  while(fgets(line,sizeof(line),infile)!=NULL)
  {
    if(num_iters == total_iters)
    {
      show_progress_bar();
      num_iters = 0;
    }
    else
      num_iters++;

    /* 1: state; 2: event; 3: comm; 4: global comm */
    switch(line[0])
    {
      case '2':
        /* we dont need id, cpu, appl, task, etc */
        /* Point to the first type */
        i = 0;
        num_char = 0;
        while(1)
        {
          if(line[i] == ':')
          {
            num_char++;
            if(num_char == 6)
            {
              line[i] = '\0';
              break;
            }
          }
          i++;
        }

        sprintf(event_record,"%s",line);

        /* Event type and values */
        end_line = 0;
        word=strtok(&line[i+1],":");
        type = atoll(word);
        word=strtok(NULL,":");
        value=atoll(word);

        while(!end_line)
        {
          /* Calculating new call id */
          char key[512];
          char *c;

          sprintf(key, "%lld:%lld",type,value);

          if((c = (char *)ht_search(hash_ids, (char *)key, strlen(key))) == NULL)
            sprintf(event_record, "%s:%lld:%lld", event_record, type, value);
          else
            sprintf(event_record,"%s:%lld:%lld", event_record, type, atoll(c));

          if((word=strtok(NULL,":"))!=NULL)
          {
            type = atoll(word);
            word=strtok(NULL,":");
            value=atoll(word);
          }
          else
            end_line = 1;
        }

        fprintf(outfile, "%s\n",event_record);

        break;

      default:
        fputs(line, outfile);
        break;
    }
  }

  /* Close the files */
  fclose(infile);
  fclose(outfile);

  ht_destroy(hash_ids);

  printf("...Done\n\n");
  printf("Trace successfully generated!!!\n\n");

  return 0;
}
