#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <unistd.h>

#define MAX_TRACE_HEADER 131072


/* Buffer for reading trace records */
char line_1[32768], line_2[32768];


/* Trace in and trace out */
FILE *infile_1, *infile_2, *outfile;


/* Parameters for showing percentage */
unsigned long long total_trace_size, current_readed_size;
double last_showed = 0;
unsigned long total_iters = 0;
int do_merge = 0;




void parse_parameters(int argc, char *argv[])
{


  if(argc == 1 || !strcmp(argv[1], "-help"))
  {
      printf("\nApplication for merging Paraver traces with the same header\n\n");
      printf("Usage:  %s < -[merge|concat] > < trace_in_1 > < trace_in_2 > < trace_out >\n\n", argv[0]);
      printf("   -merge\tMerge two traces with the same header, creating a trace ordered\n\t\tin time whit the records of the two initial traces\n");
      printf("   -concat\tConcatenate two traces with the same header\n\n");
       exit(1);
  }


  if(argc != 5) {
      printf("\nERROR: Invalid number of parameters !\n\n");
      printf("Usage:  %s < -[merge|concat] > < trace_in_1 > < trace_in_2 > < trace_out >\n\n", argv[0]);
       exit(1);
  }
                                                                                

  /* Parse params */
  if(!strcmp(argv[1], "-merge") && !strcmp(argv[1], "-concat"))
  {
     printf("\nERROR: You must specify flag -merge or -concat !\n\n");
     printf("Usage:  %s < -[merge|concat] > < trace_in_1 > < trace_in_2 > < trace_out >\n\n", argv[0]);
     exit(1);
  }
                                                                                
  if(!strcmp(argv[1], "-merge")) do_merge = 1;

}


process_trace_header(char *header1, char *header2)
{

   unsigned long long trace_time_1, trace_time_2;
   int num_comms, nano_seconds = 0;
   char *word1, *word2;


   /* Obtaining trace total time of trace 1 and 2 */

   /* Trace 1 */
   word1 = strtok(header1,":");
   fprintf(outfile, "%s:",word1);
                                                                                
   word1 = strtok(NULL, ":");
   fprintf(outfile, "%s:", word1);

   word1 = strtok(NULL, ":");
                                                                                
   if(strstr(word1, "_ns")) {
       nano_seconds = 1;
       word1[strlen(word1)-3] = '\0';
       trace_time_1 = atoll(word1);
   }
   else
       trace_time_1 = atoll(word1);

   word1 = strtok(NULL, "\n");

   /* Trace 2 */
   word2 = strtok(header2,":");
   word2 = strtok(NULL, ":");
   word2 = strtok(NULL, ":");
                                                                                
   if(strstr(word2, "_ns")) {
       word2[strlen(word2)-3] = '\0';
       trace_time_2 = atoll(word2);
   }
   else
       trace_time_2 = atoll(word2);


   if(!do_merge) trace_time_1 += trace_time_2;
   if(nano_seconds) fprintf(outfile, "%lld_ns:%s\n", trace_time_1, word1);
   else fprintf(outfile, "%lld:%s\n", trace_time_1, word1);
  

   /* Obtaining the number of communicators Trace */
   if((word1 = strrchr(word1, ',')) == NULL) return;

   strcpy(header1, word1+1);
   if(strchr(header1,')')!=NULL) return;
   num_comms = atoi(header1);
                                                                                
   while(num_comms > 0) {
        fgets(header1, MAX_TRACE_HEADER, infile_1);
        fgets(header2, MAX_TRACE_HEADER, infile_2);
        fprintf(outfile,"%s",header1);
        num_comms--;
   }
}


void ini_progress_bar(char *file_name_1, char *file_name_2) {


 struct stat file_info;


   if(stat(file_name_1, &file_info)<0) {
        perror("Error calling stat64");
        exit(1);
   }
   total_trace_size = file_info.st_size;


   if(stat(file_name_2, &file_info)<0) {
        perror("Error calling stat64");
        exit(1);
   }
   total_trace_size += file_info.st_size;



   /* Depen mida traça mostrem percentatge amb un interval diferent de temps */
   if(total_trace_size < 500000000) total_iters = 500000;
   else total_iters = 5000000;


   current_readed_size = 0;
}



void show_progress_bar() {

  double current_showed;

  current_readed_size = ftell(infile_1);
  current_readed_size += ftell(infile_2);

  current_showed = ceil(((double)current_readed_size/(double)total_trace_size)*100);


  if(last_showed != current_showed) {
       printf("...%2.0f%%", current_showed);
       last_showed = current_showed;
  }

  fflush(stdout);

}



void merge()
{
   unsigned long long time_1, time_2, num_iters = 0;
   int end_file_1, end_file_2;


   do {
                                                                                
        if(num_iters == total_iters) {
           show_progress_bar();
           num_iters = 0;
        }
        else num_iters++;
                                                                                
                                                                                
        sscanf(line_1,"%*d:%*d:%*d:%*d:%*d:%lld",&time_1);
        sscanf(line_2,"%*d:%*d:%*d:%*d:%*d:%lld",&time_2);
                                                                                
                                                                                
        while(!strcmp(line_1,line_2)) {
                end_file_2 = (fgets(line_2, sizeof(line_2), infile_2) == NULL);
                if(end_file_2) break;
        }
        if(end_file_2) break;
                                                                                
           if(time_1 <= time_2) {
                fputs(line_1, outfile);
                end_file_1 = (fgets(line_1, sizeof(line_1), infile_1) == NULL);
        }
        else {
                fputs(line_2, outfile);
                end_file_2 = (fgets(line_2, sizeof(line_2), infile_2) == NULL);
        }
   }
   while (!end_file_1 && !end_file_2);
                                                                             


   if(end_file_1)
        do {
                fputs(line_2, outfile);
        }
        while(fgets(line_2, sizeof(line_2), infile_2)!=NULL);
   else
        do {
                fputs(line_1, outfile);
        }
        while(fgets(line_1, sizeof(line_1), infile_1)!=NULL);


}



void concatenate()
{
   unsigned long long time_1, time_2, time_3, time_4, max_time_trace_1, num_iters = 0;
   int end_file_1, end_file_2, cpu, appl, task, thread, state, cpu_2, appl_2, task_2, thread_2, size, tag;
   char buffer[4096];


   /* Writing first file and obtaining max time of trace 1*/
   max_time_trace_1 = 0;
   while(fgets(line_1,sizeof(line_1),infile_1)!=NULL) {

        if(num_iters == total_iters) {
           show_progress_bar();
           num_iters = 0;
        }
        else num_iters++;


	fputs(line_1, outfile);
	sscanf(line_1, "%*d:%*d:%*d:%*d:%*d:%lld:%*s\n", &time_1);
	if(time_1 >= max_time_trace_1) max_time_trace_1 = time_1;
   }


   /* Concatenating second file */
   while(fgets(line_1,sizeof(line_1),infile_2)!=NULL) {

        if(num_iters == total_iters) {
           show_progress_bar();
           num_iters = 0;
        }
        else num_iters++;


	switch(line_1[0]) {

	   case '1': 
		     sscanf(line_1, "%*d:%d:%d:%d:%d:%lld:%lld:%d\n", &cpu, &appl, &task, &thread, &time_1, &time_2, &state);
		     time_1 += max_time_trace_1;
		     time_2 += max_time_trace_1;
		     fprintf(outfile, "1:%d:%d:%d:%d:%lld:%lld:%d\n", cpu, appl, task, thread, time_1, time_2, state);

		     break;

	   case '2': 
		     sscanf(line_1, "%*d:%d:%d:%d:%d:%lld:%s\n", &cpu, &appl, &task, &thread, &time_1, buffer);
		     time_1 += max_time_trace_1;
		     fprintf(outfile, "2:%d:%d:%d:%d:%lld:%s\n", cpu, appl, task, thread, time_1, buffer);

		     break;

	   case '3': 

		     sscanf(line_1,"%*d:%d:%d:%d:%d:%lld:%lld:%d:%d:%d:%d:%lld:%lld:%d:%d\n", &cpu, &appl, &task, &thread, &time_1, &time_2, &cpu_2, &appl_2, &task_2, &thread_2, &time_3, &time_4, &size, &tag);
		     time_1 += max_time_trace_1;
		     time_2 += max_time_trace_1;
		     time_3 += max_time_trace_1;
		     time_4 += max_time_trace_1;
		     fprintf(outfile, "3:%d:%d:%d:%d:%lld:%lld:%d:%d:%d:%d:%lld:%lld:%d:%d\n", cpu, appl, task, thread, time_1, time_2, cpu_2, appl_2, task_2, thread_2, time_3, time_4, size, tag);

		     break;

	   default: fputs(line_1, outfile); break;

	}

   }


}




int main(int argc, char *argv[]) {
 

  char *trace_name_1, *trace_name_2, *c, *word; 

  unsigned long long time_1, time_2;
  int end_file_1, end_file_2;






  /* Reading of the program arguments */
  parse_parameters(argc, argv);


  /* Open the files.  If NULL is returned there was an error */
  if((infile_1 = fopen(argv[2],"r")) == NULL) {
    perror("ERROR");
    printf("Error Opening File %s\n",argv[2]);
    exit(1);
  }


  if((infile_2 = fopen(argv[3],"r")) == NULL) {
    perror("ERROR");
    printf("Error Opening File %s\n",argv[3]);
    exit(1);
  }


  if((outfile = fopen(argv[4],"w")) == NULL) {
    printf("Error Opening File %s\n", argv[4]);
    exit(1);
  }

  

  trace_name_1 = strdup(argv[2]);
  trace_name_2 = strdup(argv[3]);


  /* Copy of the .pcf */
  c = strrchr(argv[2],'.');
  *c = '\0';


  c = strrchr(argv[4],'.');
  *c = '\0';

  sprintf(line_1,"cp %s.pcf %s.pcf\n",argv[2], argv[4]);
  system(line_1); 



  printf("\nGenerating trace %s.prv...",argv[4]);
  fflush(stdout);


  ini_progress_bar(trace_name_1, trace_name_2); 


  /* Process header */
  char *header1, *header2;

  header1 = (char *)malloc(sizeof(char) * MAX_TRACE_HEADER);
  header2 = (char *)malloc(sizeof(char) * MAX_TRACE_HEADER);

  fgets(header1, MAX_TRACE_HEADER, infile_1);
  fgets(header2, MAX_TRACE_HEADER, infile_2);

  process_trace_header(header1, header2);

  free(header1); free(header2);




  /* Processing the trace records */
  /* Merge or concatenate */
  if(do_merge) {
	 fgets(line_1, sizeof(line_1), infile_1);
         fgets(line_2, sizeof(line_2), infile_2);
	 merge();
  }
  else concatenate();

  printf("...Done\n\n");

  /* Close the files */
  fclose(infile_1); fclose(infile_2); fclose(outfile);
 
}
