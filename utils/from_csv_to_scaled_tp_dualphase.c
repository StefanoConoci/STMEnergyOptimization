#include <stdio.h>
#include <stdlib.h>
#include <string.h>



char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        *(result + idx) = 0;
    }

    return result;
}


int main(int argc, char *argv[]){
        FILE * fp;
        char * line = NULL;
        size_t len = 0;
        ssize_t read;

        int entries=0;
        double tp_baseline;
        double tp_dual;
        double tp_h0;
        double tp_h1;

        double error_baseline;
        double error_dual;
        double error_h0;
        double error_h1;

        char benchmark_name[32];
        char heuristic_name[32];
        
        char entry_line[128];

        double throughput;
        double error;

        char** tokens;



        if(argc < 2){
                printf("No input file passed as parameter\n");
                exit(EXIT_FAILURE);
        }

        fp = fopen(argv[1], "r");
        if (fp == NULL)
                exit(EXIT_FAILURE);

        int current = 0;

        while ((read = getline(&line, &len, fp)) != -1) {
                //sscanf(line, "%s,%s,%lf,%lf", &benchmark_name, &heuristic_name, &throughput, &error);
                sscanf(line, "%s", entry_line);

                if(entries > 0){    //Drop first line
                     
                    tokens = str_split(entry_line,',');
                    if (tokens){
                        int i;
                        for (i = 0; *(tokens + i); i++){
                                switch(i){
                                    case 0:
                                        strcpy(benchmark_name, *(tokens + i));
                                        break;
                                    case 1:
                                        strcpy(heuristic_name, *(tokens + i));
                                        break;
                                    case 2:  
                                        throughput = atof(*(tokens + i));
                                        break;
                                    case 3:
                                        error = atof(*(tokens + i));
                                        break;
                                }
                                free(*(tokens + i));
                            }
                            free(tokens);
                    }

                    //printf("Current= %d, benchmark_name = %s, throughput = %lf", current, benchmark_name, throughput);

                    if(current == 0){
                        tp_baseline = throughput;
                        error_baseline = error; 
                    }
                    else if(current == 1){
                        tp_dual = throughput;
                        error_dual = throughput;
                    }
                    else if(current == 2){
                        tp_h0 = throughput;
                        error_h0 = error;
                    }else if(current == 3){
                        tp_h1 = throughput;
                        error_h1 = error;
                        printf("%s\t%lf\t%lf\t%lf\t%lf\n", benchmark_name, tp_baseline/tp_baseline, tp_dual/tp_baseline, tp_h0/tp_baseline, tp_h1/tp_baseline);
                        //printf("%s\t%lf\t%lf\t%lf\n", benchmark_name, error_baseline/error_baseline, error_h0/error_baseline, error_h1/error_baseline);
                    }else{
                        printf("Current variable out of bound\n");
                        exit(1);
                    }

                    if(current == 3)
                        current = 0;
                    else current++;
                    
                }

                entries++;
         }

        fclose(fp);

        exit(EXIT_SUCCESS);
    }


