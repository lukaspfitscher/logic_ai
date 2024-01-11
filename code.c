//ctrl+shift+P for cammond line
//"editor.mouseWheelZoom": true
//write "fold all"
//settings - render whitespaces - all
//order of function arguments: data cons bfr *gt_sz
//they bigger the array size the slower the program!!!!!!!!!!!!!!!!!!!!!! factor 10000
//good value for testing 5/15/5000
//settings - rauto intent - non
//settings - detect intentation - no

//read from file some net again and therefore it didnt worked well because it was

#include "logic_ai/libs.h"
#include "logic_ai/typdef.h"
#include "logic_ai/def.h"
#include "logic_ai/var.h"
#include "logic_ai/time.h"
#include "logic_ai/random.h"
#include "logic_ai/print.h"



//size of a single input
#define inp_sz (20*bits_per_byt)
//real "variable" inputs size
int inps_sz = 3;
//lenght of the maximum training data in bits
#define data_sz_max (7300)
//max inps_sz
 //-1 becasue last inp has to out
 //+1 i dont know why make a drawing
#define inps_sz_max (data_sz_max-inp_sz)
//max number of logic units
#define max_gt_sz (3310)//max is 71000000
//max buffer (prompt in bits + luts output)
#define bfr_sz_max (inp_sz+max_gt_sz)
//inderence size in bit
#define inf_sz (20*bits_per_byt)
//input size in binary in bits
ucr data_byt[data_sz_max/bits_per_byt]={0};
//data in byte and in bit
ucr data[data_sz_max]={0};
//array consiting of single input (not used rn)
ucr inp[inp_sz]={0};
//array to save net outputs from inputs
ucr net_outs[inps_sz_max];
//net outs for inference
ucr net_outs_inf[inf_sz]={0};
//net outs for inference byte
ucr net_outs_inf_byt[inf_sz/bits_per_byt]={0};
//data in byte and in byte
ucr bfr[bfr_sz_max]={0};
//arr gt connections
cint cons[max_gt_sz][2]={0};
//arr gt connections backup
cint cons_bkp[max_gt_sz][2]={0};
//array gt lookup tabel
//ucr gt_lutb[max_gt_sz]={0b0110};
//score of the net
int out_scor=0;
//score of the net_for calculations so out score never has a wrong value 
int out_scor_calc=0;
//score of every gt
int gt_scor[max_gt_sz]={0};
//computation_score for every gt
//int gt_cmpu[max_gt_sz][30]={0};
//pointer to the last gt of the net
int gt_sz=0;
//pointer to the last gt of the net
int gt_sz_bkp=0;
//net correct (if net is correct its 1 else 0)
ucr net_correct=0;
//inp correct, if inp trough net is correct
ucr net_single_inp_correct=0;
//
ucr print_net_bit =0;
//struct test
ucr gt_outs[max_gt_sz][inps_sz_max]={0};
//
int gt_sz_last_to_file = 10000000;

long time_start=0;

int errors_allowed = 0;
int errors = 0;

//################################
//# basic logic_ai functions     #
//################################
int  bfr_ix_from_gt_ix(int gt_index){
 return gt_index+inp_sz;}

int  gt_ix_from_bfr_ix(int bfr_index){
 return bfr_index-inp_sz;}

//random net functions
int  rand_con(int bfr_ix){
 return ran_nr_inclusiv(bfr_ix-1);}//minus 1, becasue no con too itself

int  rand_gt_index(int gt_sz){
 return ran_nr_inclusiv(ix(gt_sz));}

//replace it with out_right_from_ix
void change_single_con(cint cons[][2],int gt_ix,ucr gt_inp,int new_con){
 cons[gt_ix][gt_inp]=new_con;}
ucr  out_right(ucr data[],int out_ix){
  return data[inp_sz+out_ix];}
int  net_score_from_gt_score(int gt_score[],int gt_ix){
 return gt_scor[gt_ix];}
int* net_score_ptr(int gt_score[],int gt_ix){
 return &gt_scor[gt_ix];}
ucr  net_out(ucr bfr[],int bfr_ix){
  return bfr[bfr_ix];}

ucr  net_out_from_gt_sz(ucr bfr[],cint gt_sz){}
int  last_bfr_ix(){
 return gt_sz+inp_sz-1;}

void calc_net_correct(ucr net_out,ucr net_right,ucr* net_correct){
  if(net_out==net_right) *net_correct=1;
  else *net_correct=0;}

void calc_net_single_inp_correct(ucr net_out,ucr net_right,ucr* net_single_inp_correct){
  if(equal_int(net_out,net_right)) *net_single_inp_correct=1;
  else *net_single_inp_correct=0;}

void data_byte_to_bit(ucr data_byt[],ucr data[]){
  byte_to_bit_arr(data_byt,data,data_sz_max);}

void switch_two_cint_var_with_ptr(cint* var1,cint* var2){
 int temp = *var1;
 *var1 = *var2;
 *var2 = temp;
 }

ucr  chk_two_gt_connected(cint cons[][2],int gt1_ix,int gt2_ix){
 if(bfr_ix_from_gt_ix(gt1_ix)==cons[gt2_ix][0]){return 1;}
 if(bfr_ix_from_gt_ix(gt1_ix)==cons[gt2_ix][1]){return 1;}
 return 0;}

void switch_gt_inp_cons_two_dif_gts(cint cons[][2],int gt1_ix,int gt2_ix){
 //swap_arr_rows
 for(int i=0;i<2;i++){
  int temp        = cons[gt1_ix][i];
  cons[gt1_ix][i] = cons[gt2_ix][i];
  cons[gt2_ix][i] = temp;}}

void switch_gt_out_cons_two_dif_gts(cint cons[][2],cint gt_sz,int gt1_ix,int gt2_ix){
 //gt1 has to be the sooner gate
 int gt1_bfr_ix = bfr_ix_from_gt_ix(gt1_ix);
 int gt2_bfr_ix = bfr_ix_from_gt_ix(gt2_ix);
 for(int i=0;i<gt_sz;i++){
  //seperate else if needed!
  if(     cons[i][0] == gt1_bfr_ix){cons[i][0]=gt2_bfr_ix;}
  else if(cons[i][0] == gt2_bfr_ix){cons[i][0]=gt1_bfr_ix;}
  if(     cons[i][1] == gt1_bfr_ix){cons[i][1]=gt2_bfr_ix;}
  else if(cons[i][1] == gt2_bfr_ix){cons[i][1]=gt1_bfr_ix;}
  }}

void switch_two_rand_consecutive_gts(cint cons[][2],cint gt_sz,int tms){
 if(gt_sz<=2){return;}//<2 doest work with two gates
 for(int i=0;i<tms;i++){
  //minus 1 because switching with the gate afterwards
  //minus 1 because dont switch last gate there output is pointet
  int gt_ix1 = ran_nr_exclusiv(gt_sz-1-1);
  int gt_ix2 = gt_ix1+1;
  if(chk_two_gt_connected(cons,gt_ix1,gt_ix2)){continue;}
  switch_gt_inp_cons_two_dif_gts(cons,gt_ix1,gt_ix2);
  switch_gt_out_cons_two_dif_gts(cons,gt_sz,gt_ix1,gt_ix2);
 }}

void switch_gt_out_cons_two_dif_gates_test(){
 cint cons[5][2] = {0};}

int  data_sz_from_inps_sz(int inps_sz){
 //lenght of the data
 //maximum inps_sz explanation
 //max iteratopn: minus one becasue last inp has no value to compare
 //e.g. inps_sz is 3 bit, inp_sz is 2 bit,
 //trainingdata is 5! (last training data has nothing to compare)
 return inps_sz+inp_sz;}

int  scor_max_from_inps_sz(int inps_sz){
 //max reachable score
 return inps_sz;}

int  data_sz_calc_from_inps_sz(int inps_sz){
 return inps_sz+inp_sz;}

void copy_cons_and_gt_last(cint arr_to_past[][2],cint arr_to_copy[][2],int *gt_sz_to_past,int *gt_sz_to_copy){
  copy_2d_cint_arr(arr_to_past,arr_to_copy,*gt_sz_to_copy);
  *gt_sz_to_past = *gt_sz_to_copy;}




ucr  gt_sz_max_cant_hold_an_aditional_input(cint gt_sz){
 //3 is the maximum (not, and (and not), +3 connect with net)
 //to be shure use 4
 if((gt_sz+inp_sz*4)>=max_gt_sz){return 1;}
 return 0;}

void count_algorithm_into_data(ucr arr[],int size){
 for(int i=0;i<size;i++){
  arr[i]=i%256;
  printf(" %u ",i);
 }}

void shift_left_ucr_arr(ucr* arr, size_t size){
 for (size_t i=0;i<size-1;i++){
  arr[i]=arr[i+1];}}

void last_inp_arr_from_data_arr(ucr inp_arr[],ucr data[]){
 for(size_t i=0;i<inps_sz;i++){
  inp_arr[i]=data[inps_sz_max+i];}}

ucr* last_inp_arr_last_byte(ucr data[]){
 ucr* inp_inf = malloc(inp_sz * sizeof(ucr));
 int last_byte = inps_sz-(inps_sz%8);
 for(int i=0;i<inp_sz;i++){
  inp_inf[i]=data[last_byte+i];}
 return inp_inf;}

ucr* inp_next(ucr data[],int inps_sz){
 ucr* inp_inf = malloc(inp_sz * sizeof(ucr));
 for(int i=0;i<inp_sz;i++) inp_inf[i]=data[inps_sz+inp_sz+i];
 return inp_inf;}

int bfr_ix_last_gt(){
 return bfr_ix_from_gt_ix(ix(gt_sz));}

//#######################
//# to/from file        #
//#######################

void append_to_file(const char *filename,int value){
 FILE *file = fopen(filename,"a"); // Open the file in append mode
 if (file == NULL) {
  perror("Error opening file");
  exit(EXIT_FAILURE);}
 fprintf(file, "%d\n", value); // Append the value to the file with a new line
 fclose(file);} // Close the file

void cons_from_file(ucr data_byt[]){
 FILE *file=fopen("txt","rb");
 if(file==NULL){printf("Error opening file");return;}
 fseek(file,0,SEEK_SET);
 if(!fread(data_byt,1,data_sz_max/8,file)){
  printf("data from file failed\n");}
 fclose(file);}

ucr  cons_from_file_single(cint cons[][2],cint *gt_sz){
 FILE *file = fopen("cons","r");
 if(file==NULL){printf("Error opening file");return 1;}
 int i=0;
 while(fscanf(file,"%hd %hd",&cons[i][0],&cons[i][1])==2){i++;}
 *gt_sz = i; // Set the row count
 fclose(file);
 return 0;}

ucr  cons_from_file_single_lock(cint cons[][2],int *gt_sz) {
 FILE *file;
 int fd, i = 0;
 file = fopen("cons","r");
 if (file == NULL) {
  printf("Error opening file\n");
  return 1;}
 // Get the file descriptor from the FILE* object
 fd = fileno(file);
 // Acquire an exclusive lock
 if (flock(fd, LOCK_EX) != 0) {
  printf("Error acquiring lock\n");
  fclose(file);
  return 1;}
 while (fscanf(file,"%hd %hd", &cons[i][0], &cons[i][1]) == 2){i++;}
 *gt_sz = i; // Set the row count
 // Release the lock
 flock(fd, LOCK_UN);
 fclose(file);
 return 0;}

/*void cons_from_file_with_lowest_score(cint cons[][2], int *gt_sz) {
 DIR *dir;
 struct dirent *ent;
 int lowestNumber = INT_MAX;
 char lowestFileName[256] = {0};
 dir = opendir("net/");// Open the directory
 // Iterate through files in the directory
 while ((ent = readdir(dir))!=NULL){
  int num;
  if(sscanf(ent->d_name,"%d",&num)==1){
   if(num < lowestNumber){
    lowestNumber = num;
    snprintf(lowestFileName, sizeof(lowestFileName), "net/%s", ent->d_name);}}}
 closedir(dir);
 FILE *file = fopen(lowestFileName,"r");
 int i=0;
 while(fscanf(file,"%hd %hd",&cons[i][0],&cons[i][1])==2){i++;}
 *gt_sz = i; // Set the row count
 fclose(file);}*/

ucr  cons_to_file_single(cint cons[][2],cint gt_sz){
 FILE *file=fopen("cons","w");
 if(file==NULL){printf("error opening file");return 1;}
 for(int i=0;i<gt_sz;++i){
  fprintf(file,"%d %d\n",cons[i][0],cons[i][1]);}
 fclose(file);
 return 0;}

ucr  cons_to_file_single_lock_old(cint cons[][2],cint gt_sz) {
 //change compile to gnu11 not c11 to work
 FILE *file = fopen("cons","w");
 if (file == NULL) {
  printf("Error opening file\n");
  return 1;}
 int fd = fileno(file);
 if (fd == -1) {
  printf("Error getting file descriptor\n");
  fclose(file);
  return 1;}
 // Lock the file
 if (flock(fd, LOCK_EX) == -1) {
  printf("Error locking file\n");
  fclose(file);
  return 1;}
 //write data to file
 for (int i = 0; i < gt_sz; ++i) {
  fprintf(file, "%hd %hd\n", cons[i][0], cons[i][1]);}
 // Unlock the file
 if (flock(fd, LOCK_UN) == -1) {
  printf("Error unlocking file\n");}
 //close
 fclose(file);
 return 0;}


ucr  cons_to_file_single_lock(cint cons[][2],cint gt_sz) {
 FILE *file = fopen("cons", "r+"); // Open for reading and writing
 if (file == NULL) {
  printf("Error opening file\n");
  return 1;}
 int fd = fileno(file);
 if (fd == -1){
  printf("Error getting file descriptor\n");
  fclose(file);
  return 1;}
 // Lock the file
 if (flock(fd, LOCK_EX) == -1) {
  printf("Error locking file\n");
  fclose(file);
  return 1;}
 // Clear the contents of the file after locking
 file = freopen(NULL,"w",file);
 if (file == NULL){
  printf("Error clearing file\n");
  return 1;}
 // Write data to file
 for (int i = 0; i < gt_sz; ++i) {
  fprintf(file, "%hd %hd\n", cons[i][0], cons[i][1]);}
 // Unlock the file
 if (flock(fd, LOCK_UN) == -1) {
  printf("Error unlocking file\n");}
 // Close the file
 fflush(file);
 fclose(file);
 return 0;}

void cons_to_file_single_lock_every_min(cint cons[][2],cint gt_sz){
 if(min_1min_passed()){
  cons_to_file_single_lock(cons,gt_sz);}}

/*ucr  cons_to_file_single_lock(const int cons[][2], cint gt_sz) {
 FILE *file = fopen("cons", "w");
 if (file == NULL) {
  printf("error opening file");return 1;}
 // Get the file descriptor
 int fd = fileno(file);
 // Set up the lock structure
 struct flock lock;
 lock.l_type = F_WRLCK; // Write lock
 lock.l_whence = SEEK_SET;
 lock.l_start = 0; // Start of the file
 lock.l_len = 0; // 0 means lock the entire file
 // Attempt to acquire the lock
 if (fcntl(fd, F_SETLK, &lock) == -1) {
  printf("error locking file");
  fclose(file);
  return 1;}
 // Write to the file
 for (int i = 0; i < gt_sz; ++i) {
  fprintf(file, "%d %d\n", cons[i][0], cons[i][1]);}
 // Release the lock
 lock.l_type = F_UNLCK;
 fcntl(fd, F_SETLK, &lock);
 fclose(file);
 return 0;}*/

void cons_to_file_time(cint cons[][2],cint gt_sz, long time){
 char filename[100];
 sprintf(filename, "net/%lu",time);
 FILE *file=fopen(filename,"w");
 for(int i=0;i<gt_sz;++i){
  fprintf(file,"%d %d\n",cons[i][0],cons[i][1]);}
 fclose(file);}

ucr  cons_to_file_score(cint cons[][2],cint gt_sz, int score){
 char filename[100];
 sprintf(filename, "net/%d",score);
 FILE *file=fopen(filename,"w");
 if(file==NULL){printf("error opening file");return 1;}
 for(int i=0;i<gt_sz;++i){
  fprintf(file,"%d %d\n",cons[i][0],cons[i][1]);}
 fclose(file);
 return 0;}

void data_from_file(ucr data_byt[],ucr data[]){
 cons_from_file(data_byt);
 data_byte_to_bit(data_byt,data);}

int  cnt_rows_file(const char *filename) {
 FILE *file;
 char line[1024]; // Adjust size as needed
 int count = 0;
 file = fopen(filename,"r");
 if (file == NULL) {
  perror("Error opening file");
  return -1;}// Return -1 to indicate an error
 while (fgets(line, sizeof(line), file)) {
  count++;}
 fclose(file);
 return count;}


int  cnt_rows_file_lock(const char *filename) {
 FILE *file;
 char line[1024]; // Adjust size as needed
 int count = 0;
 file = fopen(filename, "r");
 if (file == NULL) {
  perror("Error opening file");
  return -1;} // Return -1 to indicate an error
 int fd = fileno(file);
 if (fd == -1) {
  perror("Error getting file descriptor");
  fclose(file);
  return -1;}
 // Apply shared lock
 if (flock(fd, LOCK_SH) == -1) {
  perror("Error locking file");
  fclose(file);
  return -1;}
 while (fgets(line, sizeof(line), file)) {
  count++;}
 // Release the lock
 if (flock(fd, LOCK_UN) == -1) {
  perror("Error unlocking file");}
 fclose(file);
 return count;}

//#######################
//# net preperation     #
//#######################
void data_to_inputs(ucr data[],ucr inpts[][inp_sz]){
 for(int i=0;i<inp_sz;i++){
  for(int j=0;j<inp_sz;j++){
   inpts[i][j]=data[j+i*inp_sz];}}}
void zero_into_bfr(ucr bfr[],int size){
 zero_into_ucr_arr(bfr,size);}
void inp_into_bfr(ucr bfr[],ucr data[],int inp_index){
 //##copy inp into buffer
 //dont reverse it because it makes complicatat
 for(int i=0;i<inp_sz;i++){
  //printf("%d \n",bfr[i]);
  bfr[i]=data[inp_index+i];}}

void inp_into_bfr_from_inp_arr(ucr bfr[],ucr inp_arr[]){
 copy_ucr_arr(inp_arr,bfr,inp_sz);}

void reset_out_score_calc(int *out_scor_calc){
 *out_scor_calc=0;}
void reset_arr_net_outs(ucr net_outs[]){
 zero_into_ucr_arr(net_outs,inps_sz_max);}

void save_net_out_into_arr(int c_inps,ucr value,ucr output_arr[]){
 output_arr[c_inps] = value;}
void zero_into_gt_score(int gt_scor[],int arr_sz){
  zero_into_int_arr(gt_scor,arr_sz);}

//#######################
//# print out functions #
//#######################
void print_gt_sz(cint gt_sz){
 printf("gt amount:%d\n",gt_sz);}

void print_important_data(long_uint c_iterations,int gt_sz,int bfr_ix,ucr net_correct,int out_scor,int out_max,int inps_sz){
 pnt_color(green,"s");
 printf("%-*ld ",3,total_time_passed(time_start));

 pnt_color(green,"it");
 printf("%-*ld ",7,c_iterations);

 static long_uint c_iterations_last = 0;
 long_uint iteration_per_sec = c_iterations - c_iterations_last;
 c_iterations_last = c_iterations;
 pnt_color(green,"i/s");
 printf("%-*ld ",7,iteration_per_sec);
 
 pnt_color(green,"g_s");
 printf("%-*d/%d ",4,gt_sz,max_gt_sz);
 
 //printf("inp_sz:%d  ",inp_sz);
 pnt_color(green,"i_s");
 printf("%d/%d ",inps_sz,inps_sz_max);
 
 //printf("data_sz:%d/%d  ",data_sz_from_inps_sz(inps_sz),data_sz_max);
 //printf("bfr_ix:%d  ",bfr_ix);
 pnt_color(green,"cor");
 printf("%u ",net_correct);
 
 //printf("score:%d/%d  ",out_scor,out_max);
 //printf("\n");
 }

void print_in_stats(){
 //print_in_stats();
 printf("\nprint in stats:\n");
 printf("inp_sz      :%*d\n",5,inp_sz);
 printf("inps_sz_max :%*d\n",5,inps_sz_max);
 printf("max_gt_sz :%*d\n",5,max_gt_sz);
 printf("bfr_sz_max  :%*d\n",5,bfr_sz_max);
 printf("scor_max    :%*d\n",5,scor_max_from_inps_sz(inps_sz));
 printf("\n");}

void print_out_stats(cint gt_sz, int gt_scor[],int out_scor,int inps_sz){
 //print_out_stats(gt_sz,gt_scor,out_scor);
 printf("\nprint out stats:\n");
 printf("gt amt        :%*d\n",5,gt_sz);
 printf("out gt index  :%*d\n",5,ix(gt_sz));
 printf("out bfr index  :%*d\n",5,bfr_ix_from_gt_ix(ix(gt_sz)));
 printf("net score max   :%*d\n",5,scor_max_from_inps_sz(inps_sz));
 printf("net score       :%*d\n",5,out_scor);
 printf("\n");}

void print_arr_ltr_byt_only_letrs_and_numbers(ucr arr_byt[],int sz){
 //no slash therfore no new line
 for(int i=0;i<sz;i++){
  if(arr_byt[i] == ' '){
   printf("░");}
  else if(arr_byt[i]>=32 && arr_byt[i]<=126 && arr_byt[i]!=92){
   printf("%c",arr_byt[i]);}
  else{printf("#");}}}

void print_arr_ltr_byt_only_letrs_and_numbers_blue(ucr arr_byt[],int sz){
 //no slash therfore no new line
 txt_color(blue);
 print_arr_ltr_byt_only_letrs_and_numbers(arr_byt,sz);
 txt_color(reset_color);}

void print_arr_ltr_byt_only_letrs_and_numbers_violet(ucr arr_byt[],int sz){
 //no slash therfore no new line
 txt_color(violet);
 print_arr_ltr_byt_only_letrs_and_numbers(arr_byt,sz);
 txt_color(reset_color);}
 
void print_arr_ltr_bit_only_letrs_and_numbers(ucr arr_bit[]){
 ucr arr_byt[inf_sz]={0};
 bit_to_byte_arr(arr_bit,arr_byt,inf_sz);
 print_arr_ltr_byt_only_letrs_and_numbers(arr_byt,inf_sz/bits_per_byt);}

void print_arr_last_data(int tms){
 int ix_start = inps_sz+inp_sz-tms;
 if(ix_start < 0) printf("out of bounderie");
 else print_bool_arr_at_specific_ix(data,ix_start,tms);}

void print_ltr_sz8(ucr arr_byt[],int bit_ix_srt,int bit_ix_end){
 for(int i=bit_ix_srt;i<bit_ix_end;i++){
  if(i%8==0){
   if(arr_byt[i/8]>=32 && arr_byt[i/8]<=126){
    printf("[   \033[1;35m%c\033[0m  ]",arr_byt[i/8]);}
   else{printf("[   #  ]");}
   }}
 printf("\n");}

void print_data(ucr data[],int bit_ix_srt,int bit_ix_end){
 for(int i=bit_ix_srt;i<bit_ix_end;i++){
  printf("%u",data[i]);}
 printf("\n");}

void print_net_outs(ucr net_outs[],int bit_ix_srt,int bit_ix_end){
 for(int i=bit_ix_srt;i<bit_ix_end;i++){
  if(i<inp_sz){printf("-");}
  else{printf("%u",net_outs[i-inp_sz]);}}
 printf("\n");
 }

void print_bit_stream(ucr data[],ucr data_byt[],ucr net_outs[],int data_sz_calc){
 int new_line_bit_lgh = 21*8;//8 because text is 8 bit
 //int lines = division_if_rest_than_plus_one(,bit_lgh);
 //int rest_bits = %bit_lgh;
 for(int bit_ix_srt=0;bit_ix_srt<data_sz_from_inps_sz(inps_sz);bit_ix_srt+=new_line_bit_lgh){
  int bit_ix_end=bit_ix_srt+new_line_bit_lgh;
  if(bit_ix_end>data_sz_calc){bit_ix_end=data_sz_calc;}
  print_ltr_sz8     (data_byt,bit_ix_srt,bit_ix_end);
  print_data    (data    ,bit_ix_srt,bit_ix_end);
  print_net_outs(net_outs,bit_ix_srt,bit_ix_end);
 }
 printf("\n");}

void print_right_outs(ucr data[]){
 printf("print right outputs:");
 for(int i=0;i<inp_sz+(inp_sz/8);i++){printf(" ");}
 for(int i=inp_sz;i<inp_sz+inps_sz;i++){
  if(i%8==0){printf(" ");}
  printf("%u",data[i]);}
 printf("\n");}

void print_inp_from_data(ucr data[],int data_index){
 for(int data_ctr=0;data_ctr<inp_sz;data_ctr++){
  printf("%u",data[data_index+data_ctr]);}
  printf(" ");}

void print_inp_from_bfr(ucr bfr[]){
 for(int inp_ctr=0;inp_ctr<inp_sz;inp_ctr++){
  printf("%u",bfr[inp_ctr]);}
  printf(" ");}

void print_net_out(ucr bfr[],int bfr_ix){
 printf("%u",net_out(bfr,bfr_ix));}

void print_bfr_value_in_bfr_ix(ucr bfr[],int bfr_ix){
  printf("%u",bfr[bfr_ix]);}

void print_out_letter_strange(char out,ucr reset){
 //##prints out letter with a bit input stream
 //counter 0-7 (bits in byte)
 static int c=0;
 static ucr outchar=0;
 if(reset==1){c=0;outchar=0;}
 else{
  if(out==1){outchar |= (1<<(7-c));}
  if(out==0){outchar &=~(1<<(7-c));}
  c++;
  if(c==8){
   //for numbers
   //printf("%u ",outchar);
   //for chars
   if(outchar>=32 && outchar<=126){printf("%c",outchar);}
   else{printf("x");}
   c=0;}}}

void print_net_cons(cint cons[][2],int rows_until_bfr_ix){
 int columns = 10;
 int num_sz = 5;
 for(size_t i = 0; i < columns; i++){printf("---------------  ");}
 printf("\n");
 for(size_t i = 0; i < columns; i++){printf("  bfr  con  con  ");}
 printf("\n");
 for(size_t i = 0; i < columns; i++){printf("   ix    1    2  ");}
 printf("\n");
 for(size_t i = 0; i < columns; i++){printf("---------------  ");}
 
 for(int gt_ix=0;gt_ix<gt_ix_from_bfr_ix(rows_until_bfr_ix)+1;gt_ix++){
  //printf("%*d ",7,gt_ix);
  if(gt_ix%columns!=0){printf("  ");}
  else{printf("\n");}

  printf("%*d",num_sz,bfr_ix_from_gt_ix(gt_ix));
  printf("%*d",num_sz,cons[gt_ix][0]);
  printf("%*d",num_sz,cons[gt_ix][1]);
  //printf("%*d \n",5,gt_scor[i]);
  }
 printf("\n\n");}

void print_out_letters_from_bit(){
 //not working!
 for(int i=0;i<data_sz_from_inps_sz(inps_sz)/8;i++){
  data_byt[i]=0;
  for(int j=0;j<8;j++){
   data_byt[i]|=(data[i*8+j]&1)<<(7-j);}}}

//#######################
//# net checks          #
//#######################
void cmp_output_two_gts(cint cons[][2],int gt1,int gt2){
  //
 }

void chk_equal_gt_outs_arr(ucr gt_outs[][inps_sz],cint gt_sz){
 //function parameter 
 int score = 0;
 for(int gc1=0;gc1<gt_sz;gc1++){//gt_counter1
  for(int gc2=gc1;gc2<gt_sz;gc2++){//gt_counter2
   for(int goc=0;goc<gt_sz;goc++){//gt-outs_counter
    if(gt_outs[gc1][goc]==gt_outs[gc2][goc]){
     score++;
     if(score==inps_sz){
      //######### do here your shit ################
      //remove gt
      //all_cons_from_sec_gt_out_to_gt_out_first(gc1,gc2)
      //rm_gt(gc2);
      }
     }
    else{
     score=0;
     gc2++;}
 }}}}


bool chk_net_for_backwards_cons(cint cons[][2],int inp_sz_var){
 //let it run now nad than to check the net after every modification
 //if(chk_net_for_backwards_cons(cons,inp_sz)){goto end_program;}
 for(int gt_ix = 0;gt_ix<gt_sz;gt_ix++){
  for(int gt_inp_nr = 0;gt_inp_nr<2;gt_inp_nr++){
   //>= if cons to it self are allowed
   if(cons[gt_ix][gt_inp_nr] > bfr_ix_from_gt_ix(gt_ix)){
    printf("net is corrupt at bf_ix:%d, gt_inp_nr:%d, con:%d\n",
     bfr_ix_from_gt_ix(gt_ix),gt_inp_nr,cons[gt_ix][gt_inp_nr]);
    return 1;}}}
 return 0;}

//loose gate functions
void create_loose_gt_bool_arr(cint net[][2] ,int gt_sz,ucr bit_arr[]){
 //DONT!!:c_gt<gt_sz-1 becasue last gate is always lose
 zero_into_ucr_arr(bit_arr,gt_sz);
 for(int c_gt =0;c_gt<gt_sz;c_gt++){
  for(int gin =0;gin<2;gin++){
   bit_arr[gt_ix_from_bfr_ix(net[c_gt][gin])]=1;}}}

ucr  chk_if_one_loose_gt_from_loose_gt_bool_arr(cint net[][2] ,cint gt_sz, ucr bit_arr[]){
 //-1 because last gate will be always lose
 for(int c_gt =0;c_gt<gt_sz-1;c_gt++){
  if(bit_arr[c_gt] == 0){
   //printf("loose gate at gt_nr:%d\n",c_gt);
   return 1;}}
 return 0;}

cint get_gt_ix_from_loose_gt_bool_arr(cint net[][2] ,int gt_sz, ucr bit_arr[]){
 //-1 because last gate is always lose
 for(int c_gt =0;c_gt<gt_sz-1;c_gt++){
  if(bit_arr[c_gt] == 0){return c_gt;}}}

ucr  chk_loose_gt(cint net[][2],int gt_sz){
 ucr bit_arr[max_gt_sz]={0};
 create_loose_gt_bool_arr(net,gt_sz,bit_arr);
 chk_if_one_loose_gt_from_loose_gt_bool_arr(net,gt_sz,bit_arr);}

ucr  check_if_not_lu_connectet_to_lu_out(ucr net[][2],ucr net_sz,cint lu_ix){
 //lu to check if a not lu is connectet
 for(int i=lu_ix;i<net_sz;i++){
  if(net[i][0]==bfr_ix_from_gt_ix(lu_ix) && 
     net[i][1]==bfr_ix_from_gt_ix(lu_ix)){
   return 1;}}
 return 0;}



int  calc_layr_scr(cint net[][2],cint net_sz){
 int layr_arr[max_gt_sz] = {0};
 for(int i=0;i<net_sz;i++){
  int cons_layr_scr[2] = {0};
  for(int e=0;e<2;e++){
   if(net[i][e]<inp_sz){
    cons_layr_scr[e] = 0;}
   else{
    cons_layr_scr[e] = layr_arr[gt_ix_from_bfr_ix(net[i][e])];}}
  layr_arr[i] = 1+max(cons_layr_scr[0],cons_layr_scr[1]);}
 return layr_arr[ix(net_sz)];}

int  calc_layr_scr_sum(cint net[][2],cint net_sz){
 int layr_arr[max_gt_sz] = {0};
 for(int i=0;i<net_sz;i++){
  int cons_layr_scr[2] = {0};
  for(int e=0;e<2;e++){
   if(net[i][e]<inp_sz){
    cons_layr_scr[e] = 0;}
   else{
    cons_layr_scr[e] = layr_arr[gt_ix_from_bfr_ix(net[i][e])];}}
  layr_arr[i] = 1+max(cons_layr_scr[0],cons_layr_scr[1]);}
 return arr_sum(layr_arr,net_sz);}



bool chk_if_gate_has_more_than_one_output(cint net[][2],cint net_sz, cint selectet_gate_ix){
 unsigned char x = 0;
 for(int i=selectet_gate_ix;i<net_sz;i++){
  if(net[i][0]==bfr_ix_from_gt_ix(selectet_gate_ix) || net[i][0]==bfr_ix_from_gt_ix(selectet_gate_ix)) x++;
  if(x>1) return 0;}
 return 1;}

void print_number_connections_to_gt(cint net[][2],cint net_sz){
 for(int i=0;i<net_sz;i++){
  int c_con = 0;
  for(int e=i;e<net_sz;e++){
   if(i==gt_ix_from_bfr_ix(net[e][0])) c_con++;
   if(i==gt_ix_from_bfr_ix(net[e][1])) c_con++;}
  printf(" %d ",c_con);}}


//#######################
//# logic gts           #
//#######################

void gt_set(cint cons[][2],int gt_ix,int bfr_ix1,int bfr_ix2){
 cons[gt_ix][0]=bfr_ix1;
 cons[gt_ix][1]=bfr_ix2;}

void gt_set_zero(cint cons[][2],int gt_ix){
 cons[gt_ix][0]=0;
 cons[gt_ix][1]=0;}
void gt_create(cint cons[][2],int *gt_sz,int bfr_ix1,int bfr_ix2){
 (*gt_sz)++;
 cons[ix(*gt_sz)][0]=bfr_ix1;
 cons[ix(*gt_sz)][1]=bfr_ix2;}

void add_gt_not(cint cons[][2],int *gt_sz,int bfr_ix){
 gt_create(cons,gt_sz,bfr_ix,bfr_ix);}

void add_gt_nand(cint cons[][2],int *gt_sz,int bfr_ix1,int bfr_ix2){
 gt_create(cons,gt_sz,bfr_ix1,bfr_ix2);}

void add_gt_and(cint cons[][2],int *gt_sz,int bfr_ix1,int bfr_ix2){
 add_gt_nand(cons,gt_sz,bfr_ix1,bfr_ix2);
 add_gt_not(cons,gt_sz,bfr_ix_from_gt_ix(ix(*gt_sz)));}
void add_gt_or(cint cons[][2],int *gt_sz,int bfr_ix1,int bfr_ix2){
 add_gt_not(cons,gt_sz,bfr_ix1);
 add_gt_not(cons,gt_sz,bfr_ix2);
 add_gt_nand(cons,gt_sz,bfr_ix_from_gt_ix(ix(*gt_sz))-1,bfr_ix_from_gt_ix(ix(*gt_sz))-0);}

void add_rand_gt(cint cons[][2],int *gt_sz){
  gt_create(cons, gt_sz,rand_con(bfr_ix_from_gt_ix(ix(*gt_sz))),rand_con(bfr_ix_from_gt_ix(ix(*gt_sz))));}

void add_rand_gts(cint cons[][2],int *gt_sz,int amount){
 for(int i=0;i<amount;i++){
  add_rand_gt(cons,gt_sz);}}

int  gt_ix_for_rand_con_bkp = 0;
int  gt_inp_nr_for_rand_con_bkp = 0;
int  gt_inp_con_old_for_rand_con_bkp = 0;
void chang_rand_gt_single_inp_with_bkp (cint cons[][2],int gt_sz, int *gt_ix,ucr *gt_inp_nr_bkp,cint *gt_inp_con_old_bkp){
 //change the randomly the input connection of a random gate
 //pointers for easy reverse of the connection
 //*gt_ix=rand_gt_index(gt_sz);
 //*gt_inp_nr_bkp=ran_nr_exclusiv(2);
 //*gt_inp_con_old_bkp=cons[*gt_ix][*gt_inp_nr_bkp];
 //cons[*gt_ix][*gt_inp_nr_bkp]=ran_nr_inclusiv(gt_ix);
 }

void chang_rand_gt_single_inp (cint cons[][2],int gt_sz){
 //change the randomly the input connection of a random gate
 int gt_ix=rand_gt_index(gt_sz);
 int gt_inp_nr=ran_nr_exclusiv(2);
 cons[gt_ix][gt_inp_nr]=ran_nr_inclusiv(gt_ix);
 }

void chang_rand_gt_single_inp_xtms (cint cons[][2],int gt_sz, uint tms){
 //change the randomly the input connection of a random gate
 for(int i =0;i<tms;i++){
  int gt_ix=rand_gt_index(gt_sz);
  int gt_inp_nr=ran_nr_exclusiv(2);
  cons[gt_ix][gt_inp_nr]=ran_nr_inclusiv(gt_ix);}}

void chang_rand_gt_both_inps (cint cons[][2],int gt_sz){
 int gt_ix=rand_gt_index(gt_sz);
 gt_set(cons,gt_ix,
  rand_con(bfr_ix_from_gt_ix(gt_ix)),
  rand_con(bfr_ix_from_gt_ix(gt_ix)));}

void chang_rand_gt_both_inps_xtms (cint cons[][2],int gt_sz,int times){
 for(int i=0;i<times;i++){chang_rand_gt_both_inps(cons,gt_sz);}}

void chang_rand_gt_both_inps_select_only_last_gts (cint cons[][2],int gt_sz){
 int gt_ix=ran_nr_range_inclusiv(ix(gt_sz)-8*inp_sz, ix(gt_sz));
 gt_set(cons,gt_ix,
  rand_con(bfr_ix_from_gt_ix(gt_ix)),
  rand_con(bfr_ix_from_gt_ix(gt_ix)));}

void chang_rand_gt_both_inps_select_only_last_gts_xtms (cint cons[][2],int gt_sz,int times){
 for(int i=0;i<times;i++){chang_rand_gt_both_inps_select_only_last_gts(cons,gt_sz);}}

void lara_gt(cint cons[][2],int *gt_sz){
 add_gt_or(cons,gt_sz,0,0);}
//#######################
//# circuits creation   #
//#######################
void circuit_single_not_gt_at_begining(cint cons[][2],int *gt_sz){
  add_gt_not(cons,gt_sz,0);}

void circuit_single_or_gt_at_begining (cint cons[][2],int *gt_sz){
 add_gt_not(cons,gt_sz,0);
 add_gt_not(cons,gt_sz,1);
 add_gt_nand(cons,gt_sz,inp_sz+0,inp_sz+1);}

void circuit_negt_all_inp(cint cons[][2],int *gt_sz){
 for(int i=0;i<inp_sz;i++){
  add_gt_not(cons,gt_sz,i);}}

void circuit_conditional_single_and(cint cons[][2],int *gt_sz,int inp1, int inp2,ucr val1,ucr val2){
 //negts input if needed and than and (single)
 if(val1==0){add_gt_not(cons,gt_sz,inp1);inp1=bfr_ix_from_gt_ix(ix(*gt_sz));}
 if(val2==0){add_gt_not(cons,gt_sz,inp2);inp2=bfr_ix_from_gt_ix(ix(*gt_sz));}
 add_gt_and(cons,gt_sz,inp1,inp2);}

void circuit_conditional_and_total_inp(ucr data[],cint cons[][2],ucr bfr[],int *gt_sz,int inps_ix){
 inp_into_bfr(bfr,data,inps_ix);
 circuit_conditional_single_and(cons,gt_sz,0,1,bfr[0],bfr[1]);
 for(int i=2;i<inp_sz;i++){
  circuit_conditional_single_and(cons,gt_sz,bfr_ix_from_gt_ix(ix(*gt_sz)),i,1,bfr[i]);}}

void circuit_conditional_or_total_inps(cint cons[][2],int *gt_sz,ucr bfr[],ucr data[]){
 int bfr_ix1=0;
 int bfr_ix2=0;
 int inps_ctr=0;
 //printf("andor:\n");
 //printf("bfr_ix    con1    con2\n");
 //first two inputs need
 for(;inps_ctr<inps_sz;inps_ctr++){
  if(out_right(data,inps_ctr)==1){
   circuit_conditional_and_total_inp(data,cons,bfr,gt_sz,inps_ctr);
   bfr_ix1=bfr_ix_from_gt_ix(ix(*gt_sz));
   //printf("first and total inp done\n");
   inps_ctr++;
   break;}}
 
 for(;inps_ctr<inps_sz;inps_ctr++){
  if(out_right(data,inps_ctr)==1){
   circuit_conditional_and_total_inp(data,cons,bfr,gt_sz,inps_ctr);
   bfr_ix2=bfr_ix_from_gt_ix(ix(*gt_sz));
   add_gt_or(cons,gt_sz,bfr_ix1,bfr_ix2);
   //printf("%*d%*d%*d%*d\n",8,bfr_ix_from_gt_ix(ix(*gt_sz)),8,bfr_ix1,8,bfr_ix2,8,inps_ctr);
   bfr_ix1=bfr_ix_from_gt_ix(ix(*gt_sz));}}
 out_scor=inps_sz;
 }

void andor(cint cons[][2],int *gt_sz,ucr bfr[],ucr data[]){
 circuit_conditional_or_total_inps(cons,gt_sz,bfr,data);}

//#######################
//# net modifications   #
//#######################
void make_not_gt_of_nand_gt(cint cons[][2],int gt_sz){
 int gt = rand_gt_index(gt_sz);
 bool com = ran_nr_exclusiv(2);
 cons[gt][com]=cons[gt][!com];}

void chg_all_gt_out_cons_from_gt_sec_to_gt_fst(cint cons[][2],int gt_sz,int gt_1,int gt_2){
 //second name: disconnect_second_gate 
 //first it checks wich gt is first than it moves all the connections from one gt to the other gt
 //all cons from first gt to the other gt
 int bfr_ix_gt_1 = bfr_ix_from_gt_ix(gt_1);
 int bfr_ix_gt_2 = bfr_ix_from_gt_ix(gt_2);
 for(int i=0;i<gt_sz;i++){
  for(int e=0;e<2;e++){
   if(cons[i][e]==bfr_ix_gt_2){cons[i][e]=bfr_ix_gt_1;}}}}

void fuse_two_gts(cint cons[][2],int gt_sz,int gt_ix1,int gt_ix2){
 //first it checks wich gt is first than it moves all the connections from one gt to the other gt
 if(gt_ix1==gt_ix2){return;}
 //switch_bigger_last(&gt_ix1,&gt_ix2);
 chg_all_gt_out_cons_from_gt_sec_to_gt_fst(cons,gt_sz,gt_ix1,gt_ix2);}

void fuse_two_gts_rand(cint cons[][2],int gt_sz){
 ran_nr_exclusiv(gt_sz);
 fuse_two_gts(cons,gt_sz,ran_nr_exclusiv(gt_sz),ran_nr_exclusiv(gt_sz));}

void remove_backwards_cons_from_net(cint cons[][2]){
 for(int i=0;i<0;i++){
  for(int e=0;e<2;e++){
   if(cons[i][e]>bfr_ix_from_gt_ix(i)){
    cons[i][e]=bfr_ix_from_gt_ix(i);
 }}}}

void disconnect_gt_from_net(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //remove connections to the removing gt after selectet gt
 //and point to itself
 //printf("disconnect gt at ix:%d\n",bfr_ix_from_gt_ix(selct_gt_ix));
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  for(int e=0;e<2;e++){
   if(cons[i][e]==bfr_ix_from_gt_ix(selct_gt_ix)){
    //move connection to itself
    cons[i][e]=bfr_ix_from_gt_ix(i);}}}}

ucr  disconnect_gt_from_net_try(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //remove connections to the removing gate after the selectet gate
 //if bothe connections are pointing to the selectet gate break the function
 //if one con is connectet make it equal the other connection
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  if(cons[i][0]==bfr_ix_from_gt_ix(selct_gt_ix) && cons[i][1]==bfr_ix_from_gt_ix(selct_gt_ix)){
   return 1;}}
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  for(int e=0;e<2;e++){
   if(cons[i][e]==bfr_ix_from_gt_ix(selct_gt_ix)){
    cons[i][e]=cons[i][!e];}}}//con is other con
 return 0;}

void move_arr_forward_after_x_gt(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //moving gts forward after selectet gt
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  for(int e=0;e<2;e++){
   cons[i][e]=cons[i+1][e];}}}//plus! not minus!

void move_arr_backward_after_x_gt(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //moving gts backwards after selectet gt
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  for(int e=0;e<2;e++){
   cons[i][e]=cons[i-1][e];}}}//minus! not plus!

void move_cons_forward_after_x_gt(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //moving connections forward after selectet gt
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  for(int e=0;e<2;e++){
   if(cons[i][e]>bfr_ix_from_gt_ix(selct_gt_ix)){
    cons[i][e]--;}}}}

void move_cons_backwards_after_x_gt(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //moving connections backwards after selectet gt
 for(int i=selct_gt_ix;i<*gt_sz;i++){
  for(int e=0;e<2;e++){
   if(cons[i][e]>bfr_ix_from_gt_ix(selct_gt_ix)){
    cons[i][e]++;}}}}

void rmv_gt(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //printf("removing gt at ix:%d\n",bfr_ix_from_gt_ix(selct_gt_ix));
 disconnect_gt_from_net(cons,gt_sz,selct_gt_ix);
 move_cons_forward_after_x_gt(cons,gt_sz,selct_gt_ix);
 move_arr_forward_after_x_gt(cons,gt_sz,selct_gt_ix);
 gt_set_zero(cons,ix(*gt_sz));//clean up last gt
 (*gt_sz)--;
 }

void try_rmv_gt(cint cons[][2],int *gt_sz,int selct_gt_ix){
 //printf("removing gt at ix:%d\n",bfr_ix_from_gt_ix(selct_gt_ix));
 if(disconnect_gt_from_net_try(cons,gt_sz,selct_gt_ix)){
  return;}
 move_cons_forward_after_x_gt(cons,gt_sz,selct_gt_ix);
 move_arr_forward_after_x_gt(cons,gt_sz,selct_gt_ix);
 gt_set_zero(cons,ix(*gt_sz));//clean up last gt
 (*gt_sz)--;}

void add_gt(cint cons[][2],int *gt_sz,int gt_ix,int con1,int con2, int out_gt_ix, int out_gt_inp_nr){
 //not testet/working
 //keep in mind when arr is at max u cant add new gates this cold lead to errors
 (*gt_sz)++;
 move_arr_backward_after_x_gt(cons,gt_sz,gt_ix);
 move_cons_backwards_after_x_gt(cons,gt_sz,gt_ix);
 cons[gt_ix][0]=con1;
 cons[gt_ix][1]=con2;
 cons[out_gt_ix][out_gt_inp_nr]=bfr_ix_from_gt_ix(gt_ix);}

void try_rmv_rand_gt(cint cons[][2],int *gt_sz){
 try_rmv_gt(cons,gt_sz,rand_gt_index(*gt_sz));}

void try_rmv_gt_xtms(cint cons[][2],int *gt_sz,int amount){
 for(int tms=0;tms<amount;tms++){
   rmv_gt(cons,gt_sz,rand_gt_index(*gt_sz));}
 }




void check_two_gt_same_inp_adres(cint cons[][2],int *gt_sz,int *gt1,int *gt2){
 for(int gt_cmp1=0;gt_cmp1<*gt_sz;gt_cmp1++){
  for(int gt_cmp2=gt_cmp1;gt_cmp2<*gt_sz;gt_cmp2++){
   if(cons[gt_cmp1][0]==cons[gt_cmp2][0] && cons[gt_cmp1][1]==cons[gt_cmp2][1] || 
      cons[gt_cmp1][0]==cons[gt_cmp2][1] && cons[gt_cmp1][1]==cons[gt_cmp2][0] ){
    *gt1=gt_cmp1;
    *gt2=gt_cmp2;
 }}}}

void all_cons_from_sec_gt_to_first_gt(cint cons[][2],int gt_sz,int gt1,int gt2){
 for(int c_gt=gt1;c_gt<gt_sz;c_gt++){
  for(int e=0;e<2;e++){
   if(cons[c_gt][e]==gt2){cons[c_gt][e]=gt1;} 
 }}}

void remove_same_logic_from_net(cint cons[][2],int *gt_sz,int gt_cmp1,int gt_cmp2){
 int gt1_ptr=0;
 int gt2_ptr=0;
 check_two_gt_same_inp_adres(cons,gt_sz,&gt1_ptr,&gt2_ptr);
 if (gt1_ptr==0 && gt1_ptr==0){return;}
 all_cons_from_sec_gt_to_first_gt(cons,*gt_sz,gt_cmp1,gt_cmp2);
 rmv_gt(cons,gt_sz,gt_cmp2);
 }



//hash
#define hash_seq_sz (4)
ucr  gt_hash_seq[hash_seq_sz][inps_sz_max] = {0};
ucr  rand_hash_seq[inps_sz_max] = {0};
void init_rand_hash_seq(ucr arr[][inps_sz_max],int arr_sz){
 for(int i=0;i<hash_seq_sz;i++){
 rand_bit_arr(arr[i],arr_sz);}}

void calc_nand_net_with_gt_hash(cint cons[][2],ucr bfr[],int gt_sz,int gt_hash_rand_sec[][4],int gt_hash_score[][4]){
 for (int i = 0; i < gt_sz; i++){
  int sum = bfr[cons[i][0]] + bfr[cons[i][1]];
  bfr[inp_sz+i] = sum!=2;
  if(bfr[bfr_ix_from_gt_ix(i)]==gt_hash_rand_sec[i][0]){gt_hash_score[i][0]++;}
  if(bfr[bfr_ix_from_gt_ix(i)]==gt_hash_rand_sec[i][1]){gt_hash_score[i][1]++;}
  if(bfr[bfr_ix_from_gt_ix(i)]==gt_hash_rand_sec[i][2]){gt_hash_score[i][2]++;}
  if(bfr[bfr_ix_from_gt_ix(i)]==gt_hash_rand_sec[i][3]){gt_hash_score[i][3]++;}
  }}



ucr  try_rmv_one_loose_gt(cint net[][2],int *gt_sz){
 ucr lose_gt_bool_arr[max_gt_sz];
 create_loose_gt_bool_arr(net,*gt_sz,lose_gt_bool_arr);
 if(!chk_if_one_loose_gt_from_loose_gt_bool_arr(net,*gt_sz,lose_gt_bool_arr)){return 1;}
 cint gt_ix_to_remove = get_gt_ix_from_loose_gt_bool_arr(net,*gt_sz,lose_gt_bool_arr);
 //pnt_color(blue,"lose_gt_removed_gt_ix  ");
 //printf("%d\n",gt_ix_to_remove);
 try_rmv_gt(cons,gt_sz,gt_ix_to_remove);//should always work here
 //printf("one lose gt removed\n");
 //not needed for single calc
 //shift_ucr_arr_forward_after_ix(bit_arr,*gt_sz,gt_ix_to_remove);
 return 0;}

void try_rmv_all_loose_gt(cint net[][2],int *gt_sz){
 int i = 0;
 while(!try_rmv_one_loose_gt(cons,gt_sz)){i++;}
 if(i!=0){
  pnt_color(blue,"lose_gt_removed_tms  ");
  printf("%d\n",i);}}

void test_try_rmv_one_loose_gt(){
 //doesnt work bcause inp_sz is static
 cint net[4][2]= {{1,2},{2,3},{4,5},{6,7}};
 int net_sz = 3;
 try_rmv_one_loose_gt(net,&net_sz);}


void rmv_lus_with_same_inp_adr(cint net[][2],int* sz){
 //-1 last gate
 //-- becasue gate shiftet after remove
 for(int lu1=0;lu1<(*sz)-1;lu1++){
  for(int lu2=lu1+1;lu2<*sz;lu2++){
   if(net[lu1][0]==net[lu2][0] && net[lu1][1]==net[lu2][1] || net[lu1][0]==net[lu2][1] && net[lu1][1]==net[lu2][0]){
    printf("removed same gate!!!!\n");
    chg_all_gt_out_cons_from_gt_sec_to_gt_fst(net,*sz,lu1,lu2);
    rmv_gt(net,sz,lu2);
    lu2--;}}}}

void layr_nr_lus (cint net[][2],int net_sz,cint lus_layr_nr[]){
 for(int ix=0;ix<net_sz;ix++){
  //if(){}
  }}


bool scip_two_rand_cons(cint net[][2],int net_sz){
 int gt1  = rand_gt_index(net_sz);
 int cn1 = ran_nr_exclusiv(2);

 if(net[gt1][cn1]<inp_sz){return 1;}
 int gt_nxt = gt_ix_from_bfr_ix(net[gt1][cn1]);
 int cn2 = ran_nr_exclusiv(2);
 int cn_final = net[gt_nxt][cn2];
 net[gt1][cn1] = net[gt_nxt][cn2];
 return 0;}

//rmv not not circuit
bool chk_if_gt_is_not_gt(cint net[][2],int selectet_gt){
 if(net[selectet_gt][0]==net[selectet_gt][1]) return 1;
 return 0;}

bool chk_if_next_gt_exists(cint net[][2],cint net_sz,int selectet_gt){
 for (int i=selectet_gt;i<net_sz;i++){
  for (int e=0;e<2;e++){
   if(selectet_gt == gt_ix_from_bfr_ix(net[i][e])) return 1;
 return 0;}}}

int  clc_out_cons(cint net[][2],cint net_sz,int selectet_gt){
 int x = 0;
 for (int i=selectet_gt;i<net_sz;i++){
  for (int e=0;e<2;e++){
   if(selectet_gt == gt_ix_from_bfr_ix(net[i][e])) x++;
 return x;}}}

int  calc_first_next_gt(cint net[][2],cint net_sz,int selectet_gt){
 for (int i=selectet_gt;i<net_sz;i++){
  for (int e=0;e<2;e++){
   if(selectet_gt == gt_ix_from_bfr_ix(net[i][e])) return i;}}}

bool disconnect_double_not_gts(cint net[][2],cint net_sz){
 //check if gt is not gt
 //clc next gt
 //check if gt is not gt
 //move al con to first gt 
 for (int i=0;i<net_sz-1;i++){
  //printf("imhere\n");
  if(!chk_if_gt_is_not_gt(net,i)) continue;
  //if(clc_out_cons(net,net_sz,i)==2) return 1;
  int x = calc_first_next_gt(net,net_sz,i);
  if(!chk_if_gt_is_not_gt(net,x)) continue;
  chg_all_gt_out_cons_from_gt_sec_to_gt_fst(net,net_sz,gt_ix_from_bfr_ix(net[i][0]),x);
  pnt_color(blue,"double_not_gt_removed\n");}}

//local net

void get_local_net(cint net[][2],int net_sz,int gt_ix,int local_net_sz, int gts_in_local_net[]){}

/*void get_gt_outs_arr(cint net[][2],int gt_ix,ucr bfr[],ucr gt_out_arr[]){
for(int i=0;i<sz(gt_ix);i++){
 inp_into_bfr();
 zero_into_bfr();
 calc_nand_net_efficient();
 gt_out_arr[i]=bfr[bfr_ix_from_gt_ix(gt_ix)];}}*/

//#######################
//# net calc            #
//#######################
ucr  inp1_value_nand_gt(ucr bfr[],cint cons[][2],int gt_to_calc){
 return bfr[cons[gt_to_calc][0]];}

ucr  inp2_value_nand_gt(ucr bfr[],cint cons[][2],int gt_to_calc){
 return bfr[cons[gt_to_calc][1]];}
ucr  nand_calc(ucr i1,ucr i2){
 if(i1==1 && i2==1){return 0;}
 else{return 1;}}
//you only need this!!!!!
void calc_nand_net_clean(cint cons[][2],ucr bfr[],int gt_sz){
 for(int gt_to_calc=0;gt_to_calc<gt_sz;gt_to_calc++){
  bfr[inp_sz+gt_to_calc]=
   nand_calc(
    inp1_value_nand_gt(bfr,cons,gt_to_calc),
    inp2_value_nand_gt(bfr,cons,gt_to_calc));}}

void calc_nand_net_efficient(cint cons[][2],ucr bfr[],int gt_sz){
 for (int i = 0; i < gt_sz; i++){
  int sum = bfr[cons[i][0]] + bfr[cons[i][1]];
  bfr[inp_sz+i] = sum!=2;}}

ucr  calc_nand_net_return(cint cons[][2],ucr bfr[],int gt_sz){
 calc_nand_net_efficient(cons,bfr,gt_sz);
 return bfr[last_bfr_ix()];}

ucr  calc_nand_net_with_input_return(cint cons[][2],ucr bfr[],int gt_sz,ucr inp[]){
 //not finished input into buffr!!!!!!!!!!!!!!!!!!!!
 return calc_nand_net_return(cons,bfr,gt_sz);}

ucr  calc_nand_net_without_bfr__return(cint cons[][2],int gt_sz){
 ucr* bfr = malloc((gt_sz+inp_sz) * sizeof(ucr));
 for (int i = 0; i < gt_sz; i++){
  int sum = bfr[cons[i][0]] + bfr[cons[i][1]];
  bfr[inp_sz+i] = sum!=2;}
 int net_out = bfr[last_bfr_ix()];
 free(bfr);
 return net_out;}

void calc_nand_net_efficient_old_v2(cint cons[][2],ucr bfr[],int g_amt){
 for(int i=0;i<g_amt;i++){
  if(bfr[cons[i][0]]+ bfr[cons[i][1]]==2){
   bfr[inp_sz+i]=0;}
  else{
   bfr[inp_sz+i]=1;}}}

void calc_nand_net_efficient_old_v1(cint cons[][2],ucr bfr[],int g_amt){
 for(int i=0;i<g_amt;i++){
  if(bfr[cons[i][0]]==1 && 
     bfr[cons[i][1]]==1){
   bfr[inp_sz+i]=0;}
  else{
   bfr[inp_sz+i]=1;}}}

void calc_score_from_net_out(ucr net_out,int out_right,int *score){
 //only for test purposes after not needed because net will be always right
 if(net_out==out_right){(*score)++;}}

void calc_score_from_net_correct(int *score,int net_correct){
//to know bassicly how far the net got
 //only for test purposes after not needed because net will be always right
 if(net_correct==1)(*score)++;}



ucr  chk_if_there_are_cons_to_itself(cint net[][2],int sz){
 //if(chk_if_there_are_cons_to_itself(cons,gt_sz)){printf("cons_to_itself\n");goto end_program;}
 for (int i=0;i<sz;i++){
  for (int e=0;e<2;e++){
   if(net[i][e]==bfr_ix_from_gt_ix(i)){return 1;}}}
 return 0;}

int  cnt_cons_to_itself(cint net[][2],int sz){
 //if(cnt_cons_to_itself(cons,gt_sz)){printf("cons_to_itself\n");goto end_program;}
 int cnt =0;
 for (int i=0;i<sz;i++){
  for (int e=0;e<2;e++){
   if(net[i][e]==bfr_ix_from_gt_ix(i)){cnt++;}}}}

ucr  chk_if_there_both_gt_cons_to_itself(cint net[][2],int sz){
 //if(chk_if_there_both_gt_cons_to_itself(cons,gt_sz)){printf("cons_to_itself\n");goto end_program;}
 for (int i=0;i<sz;i++){
   if(net[i][0]==bfr_ix_from_gt_ix(i) && net[i][1]==bfr_ix_from_gt_ix(i)){
    return 1;}}
 return 0;}

//######################
//# limits check       #
//######################
ucr  chk_min_gt_sz_from_inp_sz_and_inps_sz(){
 if(max_gt_sz<(inps_sz*inp_sz*3)){
  printf("max_gt_sz_to_small, use: %d\n",(inps_sz*inp_sz*3));
  return 1;}
 return 0;}

ucr  chk_inps_bigr_data_sz(){
 if(inps_sz>data_sz_max){
  printf("inps_sz bigger than data_sz_max\n");
  return 1;}
 return 0;}

ucr  chk_equal_inputs(ucr data[]){
 //check equal inputs
 ucr inp_check[inp_sz]={0};
 //loop selecting input data
 for(int i=0;i<=inps_sz_max;i++){
  //printf("next:%*d\n",5,i);
  //copying new inp data from data
  for(int j=0;j<inp_sz;j++){inp_check[j]=data[i+j];}
  //counter for running input trogh data
  for(int e=i+1;e<=inps_sz_max;e++){
   int cnt_similar=0;
   //calc if datasnip is similar to input snip
   for(int s=0;s<inp_sz;s++){if(inp_check[s]==data[e+s]){cnt_similar++;}}
   //check if same and reset cnt
   if(cnt_similar==inp_sz){
    printf("\nequal inp at:%d!\n",e);
    return 1;}}}
 return 0;}

ucr  chk_one_in_first_outs(ucr data[]){
 for(int i=0;i<inps_sz;i++){
  //printf(" %u ",out_right(data,i));
  if(out_right(data,i)==1){
   return 0;}}
 printf("only zeros in first outs!\n");
 return 1;}

ucr  chk_max_and_limits(ucr data[]){
 if(chk_min_gt_sz_from_inp_sz_and_inps_sz()){return 1;}
 if(chk_inps_bigr_data_sz()){return 1;}
 if(chk_equal_inputs(data)){return 1;}
 if(chk_one_in_first_outs(data)){return 1;}
 return 0;}
  
//######################
//# main functions     #
//######################
void init_basics(long* time_start){
 init_random_gen_by_time();
 init_timer(time_start);}

ucr  net_reduced_by_factor(int reduction){
 //printf(" %d %d",out_scor,gt_sz);
 //printf(" %d \n",out_scor/(gt_sz)<reduction);
 //return inp_sz/(gt_sz)<reduction;}
 if(data_sz_from_inps_sz(inps_sz)>=data_sz_max){return 0;}
 if(gt_sz<(inps_sz/reduction)){return 1;}else{return 0;};}

void set_print_and_save_net(ucr *print_net_bit){
 (*print_net_bit)=1;}

void try_add_new_inp_as_long_as_net_is_correct(ucr bfr[],ucr data[],cint cons[][2],int gt_sz,int *inps_sz,int *out_scor){

 //when modifing check calc_inps_sz_and_out_scor!
 int i = 0;
 while(1){
  if(*inps_sz>=inps_sz_max){break;}
  zero_into_bfr(bfr,bfr_sz_max);
  inp_into_bfr(bfr,data,next(ix(*inps_sz)));
  calc_nand_net_efficient(cons,bfr,gt_sz);
  if(net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz)))!=out_right(data,next(ix(*inps_sz)))){break;}
  (*inps_sz)++;
  (*out_scor)++;
  i++;}
 if(i!=0){
  pnt_color(orange,"new_single_inp_added         ");
  printf("%d\n",i);}}

void calc_inps_sz_and_out_scor_from_net(ucr bfr[],ucr data[],cint cons[][2],int gt_sz,int *inps_sz,int *out_scor){
 try_add_new_inp_as_long_as_net_is_correct(bfr,data,cons,gt_sz,inps_sz,out_scor);}

void add_new_inp_to_net(ucr data[],cint cons[][2],ucr bfr[],int *gt_sz,int *inps_sz,int *out_scor){
 //before check if its even neccesary and remove outright==outnet condition
 try_add_new_inp_as_long_as_net_is_correct(bfr,data,cons,*gt_sz,inps_sz,out_scor);
 //if(out_scor/(*gt_sz)<2){return;}
 //if((*gt_sz)/out_scor<4){return;}
 
 if(*inps_sz>=inps_sz_max){return;}
 if(gt_sz_max_cant_hold_an_aditional_input(*gt_sz)){return;}
 (*inps_sz)++;
 (*out_scor)++;
 int inp1=bfr_ix_from_gt_ix(ix(*gt_sz));//bfr_ix net out
 circuit_conditional_and_total_inp(data,cons,bfr,gt_sz,ix(*inps_sz));
 //because otherwise add input would be positiv
 //int net_out = !out_right(data,ix(*inps_sz));
 if(out_right(data,ix(*inps_sz))==1){
  add_gt_or(cons,gt_sz,inp1,bfr_ix_from_gt_ix(ix(*gt_sz)));}
 else{
  //int inp2 = bfr_ix_from_gt_ix(ix(*gt_sz));
  add_gt_not(cons,gt_sz,bfr_ix_from_gt_ix(ix(*gt_sz)));//negate andor
  add_gt_and(cons,gt_sz,inp1,bfr_ix_from_gt_ix(ix(*gt_sz)));}
 printf("new inp added\n");
 //rmv_lus_with_same_inp_adr(cons,gt_sz);
 }

void apply_strategie(ucr data[],cint cons[][2],ucr bfr[],int *gt_sz,int *inps_sz,int c_main,ucr *print_net_bit,int* out_scor){
 //printf(" gs:%d,%d ",c_main%nr_strat,out_scor);
 int strat_nr = c_main % 8;
 //if(c_main%20000==0){add_new_inp_to_net(data,cons,bfr,gt_sz,inps_sz,out_scor);}
 //if(net_reduced_by_factor(1) && !gt_sz_max_cant_hold_an_aditional_input(*gt_sz)){// 
 //  add_new_inp_to_net(data,cons,bfr,gt_sz,inps_sz,out_scor);}
 if     (strat_nr==0){try_rmv_rand_gt(cons,gt_sz);}
 else if(strat_nr==1){chang_rand_gt_both_inps_xtms(cons,*gt_sz,1);}//they are needed!!!!
 else if(strat_nr==2){chang_rand_gt_both_inps_xtms(cons,*gt_sz,2);}//they are needed!!!!
 else if(strat_nr==3){chang_rand_gt_single_inp_xtms(cons,*gt_sz,1);}
 else if(strat_nr==4){chang_rand_gt_single_inp_xtms(cons,*gt_sz,2);}//they are needed!!!!
 else if(strat_nr==5){chang_rand_gt_single_inp_xtms(cons,*gt_sz,3);}
 else if(strat_nr==6){chang_rand_gt_single_inp_xtms(cons,*gt_sz,4);}
 else if(strat_nr==7){chang_rand_gt_single_inp_xtms(cons,*gt_sz,5);}
 //else if(strat_nr==6){chang_rand_gt_both_inps_select_only_last_gts_xtms(cons,*gt_sz,1);}
 //else if(strat_nr==7){make_not_gt_of_nand_gt(cons,*gt_sz);}
 }

void calc_net_all_inps_save_output_into_array_break_if_not_right(
 ucr data[],cint cons[][2],ucr bfr[],int gt_sz,int *out_scor,int *out_scor_calc,ucr *net_correct,int inps_sz){
 //printf("cal net whole inps starts: %d\n", c_main);
 reset_arr_net_outs(net_outs);
 reset_int(out_scor_calc);
 for(int c_inps=0;c_inps<inps_sz;c_inps++){
  zero_into_bfr(bfr,bfr_sz_max);
  inp_into_bfr(bfr,data,c_inps);
  calc_nand_net_efficient(cons,bfr,gt_sz);
  calc_net_correct(net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz))),out_right(data,c_inps),net_correct);
  if(*net_correct==0) return;
  save_net_out_into_arr(c_inps,net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz))),net_outs);
  calc_score_from_net_correct(out_scor_calc,*net_correct);
  //calc_score_from_net_out(net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz))),out_right(data,c_inps),out_scor_calc);
  }
 *out_scor=*out_scor_calc;}


void calc_net_save_output_into_array_break_if_over_error_threshhold(int *out_scor,int *out_scor_calc,ucr *net_correct){
 reset_arr_net_outs(net_outs);
 reset_int(out_scor_calc);
 for(int c_inps=0;c_inps<inps_sz;c_inps++){
  inp_into_bfr(bfr,data,c_inps);
  calc_nand_net_efficient(cons,bfr,gt_sz);
  calc_net_correct(net_out(bfr,bfr_ix_last_gt()),out_right(data,c_inps),&net_single_inp_correct);
  if(errors>errors_allowed) return;
  save_net_out_into_arr(c_inps,net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz))),net_outs);
  calc_score_from_net_correct(out_scor_calc,*net_correct);}
 *out_scor=*out_scor_calc;}



void reverse_net_if_false(cint cons[][2],cint cons_bkp[][2],int *gt_sz,int *gt_sz_bkp,int net_correct){
 if(net_correct==0){
  copy_cons_and_gt_last(cons,cons_bkp,gt_sz,gt_sz_bkp);}}

void reverse_net_if_over_error_threshhold(int *gt_sz,int *gt_sz_bkp){
 if(errors>errors_allowed)copy_cons_and_gt_last(cons,cons_bkp,gt_sz,gt_sz_bkp);}

void reverse_net_if_smaller_layr_scr(cint cons[][2],cint cons_bkp[][2],int *gt_sz,int *gt_sz_bkp){
 static long layr_scr_sum_bfr = 0;
 long layr_scr_sum = calc_layr_scr_sum(cons,*gt_sz);
 if(layr_scr_sum>layr_scr_sum_bfr){
  copy_cons_and_gt_last(cons,cons_bkp,gt_sz,gt_sz_bkp);}
  layr_scr_sum_bfr = layr_scr_sum;}

void inp_inf_next(ucr inf_inp[],ucr net_out_var){
 shift_left_ucr_arr(inf_inp,inp_sz);
 inf_inp[ix(inp_sz)] = net_out_var;}

void inference(ucr inf_inp[],ucr net_outs_inf[],int tms){
 for(size_t i=0;i<tms;i++){
  zero_into_bfr(bfr,bfr_sz_max);
  inp_into_bfr_from_inp_arr(inf_inp,bfr);
  calc_nand_net_efficient(cons,bfr,gt_sz);
  int net_out_var=net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz)));
  net_outs_inf[i]=net_out_var;
  inp_inf_next(inf_inp,net_out_var);}}

ucr* inference_return(ucr inf_inp[],int tms){
 ucr* x = malloc(tms * sizeof(ucr));
 for(size_t i=0;i<tms;i++){
  zero_into_bfr(bfr,bfr_sz_max);
  inp_into_bfr_from_inp_arr(inf_inp,bfr);
  calc_nand_net_efficient(cons,bfr,gt_sz);
  int net_out_var=net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz)));
  x[i]=net_out_var;
  inp_inf_next(inf_inp,net_out_var);}
 return x;}

void print_inference_last_data(int times){
 ucr inf_inp[inp_sz] = {0};
 last_inp_arr_from_data_arr(inf_inp,data);
 for(size_t i=0;i<times;i++){
  zero_into_bfr(bfr,bfr_sz_max);
  inp_into_bfr_from_inp_arr(inf_inp,bfr);
  calc_nand_net_efficient(cons,bfr,gt_sz);
  int net_out_var=net_out(bfr,bfr_ix_from_gt_ix(ix(gt_sz)));
  printf("%u",net_out_var);
  shift_left_ucr_arr(inf_inp,inp_sz);
  inf_inp[ix(inp_sz)] = net_out_var;}
 printf("\n");}

//ucr inp_test_byt[inp_sz/8]="three is green and l";
ucr inp_test_byt[inp_sz/8]=  "                  th";
ucr inp_test[inp_sz]={0};//testdata in bit
void print_inferenc_custom_data(){
  //testdata in byte eg for inference
  //                          12345678901234567890
  //ucr inp_test_byt[inp_sz/8]="what is a star und w";
  //ucr inp_test_byt[inp_sz/8]="The world is a vast ";
  //ucr inp_test_byt[inp_sz/8]="languages, and tradi";
  //txt_color(green();
  pnt_color(orange,"inf_custom                   ");
  print_arr_ltr_byt_only_letrs_and_numbers_violet(inp_test_byt,inp_sz/bits_per_byt);
  //txt_color_rest();
  byte_to_bit_arr(inp_test_byt,inp_test,inp_sz);
  inference(inp_test,net_outs_inf,inf_sz);
  txt_color(yellow);
  print_arr_ltr_bit_only_letrs_and_numbers(net_outs_inf);
  txt_color(reset_color);
  printnl();
 }


void print_net_data(ucr data[],ucr data_byt[],cint cons[][2],int gt_sz,int inps_sz, long_uint c_main,int out_scor,ucr net_correct){
 //print_net_cons(cons,bfr_ix_from_gt_ix(ix(gt_sz)));
 //print_bit_stream(data,data_byt,net_outs,data_sz_calc_from_inps_sz(inps_sz));
 //print_inference(40);
 //printf("\n");
 print_important_data(c_main,gt_sz,bfr_ix_from_gt_ix(ix(gt_sz)),net_correct,out_scor,scor_max_from_inps_sz(inps_sz),inps_sz);
 }

#define tms_after (1000)

ucr arr_net_outs_inps_after[tms_after] = {0};
void get_arr_net_outs_inps_after(int tms){
 if(tms>tms_after) pnt_color(red,"arr_net_outs_inps_after_too_big");
 //printf("cal net whole inps starts: %d\n", c_main);
 for(int i=0;i<tms;i++){
  inp_into_bfr(bfr,data,ix(inps_sz)+1+i);//+1 because after (not last)
  arr_net_outs_inps_after[i]=calc_nand_net_return(cons,bfr,gt_sz);}}

ucr arr_out_after[tms_after] = {0};
void get_arr_out_after(int tms){
 //array with data outputs after
 if(tms>tms_after) pnt_color(red,"arr_out_after_too_big ");
 for(int i=0;i<tms;i++){
  arr_out_after[i]=data[inps_sz+inp_sz+i];}}

void print_net_right_data_aftr_sc(int tms){
 //inferences next inputs
 pnt_color(green,"data_aftr_sc");
 get_arr_out_after(tms);
 get_arr_net_outs_inps_after(tms);
 int score = similar_score_two_bool_arrs(arr_net_outs_inps_after,arr_out_after,tms);
 append_to_file("net_goodness",score);
 printf("%-*d ",4,score);}

void print_inf_sc_data_aftr(int tms){
 pnt_color(green,"inf_sc");
 get_arr_out_after(tms);
 printf("%-*d ",4,
  similar_score_two_bool_arrs(
   inference_return(inp_next(data,inps_sz),tms),
   arr_out_after,tms));}




void print_inference_last_byte_data(){
 pnt_color(green,"inf");
 inference(last_inp_arr_last_byte(data),net_outs_inf,inf_sz);
 txt_color(yellow);
 print_arr_ltr_bit_only_letrs_and_numbers(net_outs_inf);
 txt_color(reset_color);
 pnt(" ");}










//######################
//# main               #
//######################
long_uint c_max = max_sz_long_uint; //1;
int main(){
 init_basics(&time_start);

 data_from_file(data_byt,data);
 //count_algorithm_into_data(data_byt,data_sz_max/8+1);
 //data_byte_to_bit(data_byt,data);

 if(chk_max_and_limits(data)){goto end_program;}

 //cons_from_file_single_lock(cons,&gt_sz);
 andor(cons,&gt_sz,bfr,data);

 calc_inps_sz_and_out_scor_from_net(bfr,data,cons,gt_sz,&inps_sz,&out_scor);

 print_inferenc_custom_data();

 //bit_to_byte_arr(net_outs_inf,net_outs_inf_byt,inf_sz);
 //print_arr_ltr_byt_only_letrs_and_numbers_blue(net_outs_inf_byt,inf_sz/bits_per_byt);
 //print_ltr(ucr data_byt[],int bit_ix_srt,int bit_ix_end)

 printnl();



 for(long_uint c_main=0;c_main<c_max;c_main++){
 //printf("it:%ld\n",c_main);
  //logic optimisation
  if(c_main%1000){
   //disconnect_double_not_gts(cons,gt_sz);
   try_rmv_all_loose_gt(cons,&gt_sz);
   switch_two_rand_consecutive_gts(cons,gt_sz,20);
   try_add_new_inp_as_long_as_net_is_correct(bfr,data,cons,gt_sz,&inps_sz,&out_scor);
   if(gt_sz<1000){
    add_new_inp_to_net(data,cons,bfr,&gt_sz,&inps_sz,&out_scor);}
   }

  
  if(min_1sec_passed(time_start)) set_print_and_save_net(&print_net_bit);

  //core
  copy_cons_and_gt_last(cons_bkp,cons,&gt_sz_bkp,&gt_sz);
  if(!print_net_bit) apply_strategie(data,cons,bfr,&gt_sz,&inps_sz,c_main,&print_net_bit,&out_scor);
  calc_net_all_inps_save_output_into_array_break_if_not_right(data,cons,bfr,gt_sz,&out_scor,&out_scor_calc,&net_correct,inps_sz);
  reverse_net_if_false(cons,cons_bkp,&gt_sz,&gt_sz_bkp,net_correct);
  //reverse_net_if_smaller_layr_scr(cons,cons_bkp,&gt_sz,&gt_sz_bkp);
  
  /*
  if(gt_sz<gt_sz_last_to_file && out_scor==inps_sz_max){//out_scor its a savety machanism so corrupt nets will not be saved
   cons_to_file_single_lock(cons,gt_sz);
   gt_sz_last_to_file = gt_sz;}
  */
  
   
  if(print_net_bit){
   cnt_cons_to_itself(cons,gt_sz);
   reset(&print_net_bit);
   print_net_data(data,data_byt,cons,gt_sz,inps_sz,c_main,out_scor,net_correct);
   
   //inference(last_inp_arr(data),net_outs_inf,inf_sz);
   
   print_inference_last_byte_data();
   
   //print_inf_sc_data_aftr(tms_after);
   print_net_right_data_aftr_sc(tms_after);

   //pnt_color(green,"l_scr_last_gt");
   //printf("%-*d ",4,calc_layr_scr(cons,gt_sz));

   pnt_color(green,"l_scr_sum");
   printf("%-*d ",5,calc_layr_scr_sum(cons,gt_sz));
   //print_number_connections_to_gt(cons,gt_sz);

   pnt_color(green,"last_data");
   print_arr_last_data(5);

   

   printnl();
   // rows = cnt_rows_file_lock("cons");
   //if(rows<gt_sz && rows>0){
   // cons_from_file_single_lock(cons,&gt_sz);
   // printf("better net from file!, gt_sz:%d\n",gt_sz);}
   
   cons_to_file_single_lock_every_min(cons,gt_sz);
   if(!net_correct){printf("nett incorrect\n");goto end_program;}
   }
  }

 end_program: 
 printf("program finished\n");}