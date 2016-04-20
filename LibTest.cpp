/*SKELETON CODES FOR AVC */
/*  A.Roberts, 5 Apr 2016*/
#include <stdio.h>

//#include <pthread.h>
#include <time.h>
#include <string.h>

// sudo gcc -Wall
extern "C" int init_hardware();
extern "C" int init(int d_lev);

extern "C" int take_picture();
extern "C" char get_pixel(int row, int col, int color);
extern "C" void set_pixel(int col, int row, char red,char green,char blue);

extern "C" int open_screen_stream();
extern "C" int close_screen_stream();
extern "C" int update_screen();
//extern "C" void GetLine(int row,int threshold);
extern "C" int display_picture(int delay_sec,int delay_usec);
extern "C" int save_picture(char filename[5]);

extern "C" int set_motor(int motor,int speed);

extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);
extern "C" int select_IO(int chan, int direct);
extern "C" int write_digital(int chan,char level);
extern "C" int read_digital(int chan);
extern "C" int set_PWM(int chan, int value);

extern "C" int connect_to_server( char server_addr[15],int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);


#define NO_LINE -500000  // value returned if no line detected

// Debug file
FILE *deb_file;
int quadrant;


// returns  error propottion to white line displacement
// from the center of camera view
// y - pixels row number
// n_tp - number of test points
// threshold - decision on white
// returns -10000 if no white pixels were detected
int LineError(int y_tp, int n_tp, int threshold)
{
   int err = 0;
   // test points
   int x_tp[124];         // horizontal coordinates of test points
   int white_tp[124];     // luminocity at test points
   int white_grad[124];   // gradient of luminocity
   int white_tp_thr[124]; // robust decision about luminocity
   int n_whites = 0;
   int i_tp = 0 ;
   int width = 320;
   int dx  =  width / n_tp;

   take_picture();
   for (i_tp = 0 ; i_tp < n_tp ; i_tp++)
   {
        x_tp[i_tp] = dx/2 + i_tp*dx;
        white_tp[i_tp] = get_pixel(x_tp[i_tp],y_tp,3);
        white_tp_thr[i_tp] = 0;
        if ( white_tp[i_tp] > threshold )
        {
           white_tp_thr[i_tp] =1;
           n_whites = n_whites + 1;
           err = err + (i_tp - (n_tp - 1)/2) *10*white_tp_thr[i_tp];
        }
   }
   // display only
   for (i_tp=0; i_tp < n_tp ; i_tp++)
   {
      printf("%d ",white_tp[i_tp]);
   }
   printf("\n");

   for (i_tp=0; i_tp < n_tp ; i_tp++)
   {
       printf("%d ",white_tp_thr[i_tp]);
   }
   printf("\n");
   printf(" err=%d\n",err);
   if (n_whites == 0)
   {
     return NO_LINE;
   }
   return err;
}


int main()
{
    init(1);
    quadrant = 0;
    char fn[6]={'f','r','a','m','e','0'};
    //take_picture();
    //display_picture(1,0);
    //open_screen_stream();

    int v_left = 0;
    int v_right = 0;


    // CHECK network
     /*
    char server_addr[15];
    sprintf(server_addr,"130.195.5.173");
    connect_to_server(server_addr,1024);
    char greeting[24];
    sprintf(greeting,"Please");
    send_to_server(greeting);
    char pswd_rcvd[24];
    receive_from_server(pswd_rcvd);
    printf("pswd=%s\n",pswd_rcvd);
    char pswd_transm[24];
    strncpy(pswd_transm,pswd_rcvd,6);
    //pswd_transm[6] = '\0';
    send_to_server(pswd_transm);

    return 0;
     */

    //clock_t start_t,curr_t;
    struct timespec start;
    struct timespec now;
    deb_file = fopen("deb.txt","w+");

    int err =-10000;

    int countp = 0;
    clock_gettime(CLOCK_REALTIME,&start);

    while(countp<2500)
    {
      err = LineError(100, 32, 120);

     // take_picture();
      //SavePicture(fn);
      //n_whites = 0;
     // for (i_tp=0; i_tp<8;i_tp++)
     // {
     //   set_pixel(x_tp[i_tp],  y_tp+2,255,0,0);
     //   set_pixel(x_tp[i_tp]-1,y_tp+2,255,0,0);
     //   set_pixel(x_tp[i_tp]+1,y_tp+2,255,0,0);
     //   set_pixel(x_tp[i_tp],  y_tp-2,255,0,0);
     //   set_pixel(x_tp[i_tp]-1,y_tp-2,255,0,0);
     //   set_pixel(x_tp[i_tp]+1,y_tp-2,255,0,0);
     // }
     // update_screen();
      if ( err == NO_LINE )
      {
         printf("Line lost\n");
         v_left = -35;
         v_right = -35;
         set_motor(1,v_right);
         set_motor(2,v_left);
         Sleep(0,100000);
      } else
      {
         v_left = 35 - err*0.01;
         v_right = 35 + err*0.01;
         set_motor(1,v_right);
         set_motor(2,v_left);
      }
      clock_gettime(CLOCK_REALTIME,&now);
      double tim = now.tv_sec - start.tv_sec;
      tim = tim + (now.tv_nsec - start.tv_nsec)/1000000000.0;
      printf("  tim=%f  err=%d v_left=%d v_right=%d\n",tim,err,v_left,v_right);
      fprintf(deb_file,"%f %d %d %d\n",tim, err,v_left,v_right);
      fflush(deb_file);
      printf("\n");
      //Sleep(0,50000);
      countp++;
    }
//    save_picture(fn);
//    close_screen_stream();
    set_motor(1,0);
    set_motor(2,0);
    return 0;





    printf("start motor test\n");
    int im = 0;
    while(im<3)
    {
      printf("im=%d out of 3\n",im);
      set_motor(1,125);
      Sleep(2,0);
      set_motor(1,-125);
      Sleep(2,0);
      set_motor(1,0);

      im++;
    }
    printf("end");


    /*****************CHECK ANALOG INPUTS********/
    for (int ai = 0 ; ai < 8; ai++)
    {
        int av = read_analog(ai);
        printf("ai=%d av=%d\n",ai,av);
    }
    Sleep(1,0);


    /***********CHECK DIGITAL OUTPUTS*****/
    int pin;
    for (pin=0; pin < 8; pin++)
    {
      select_IO(pin, 0);
    }
    int count = 0;
    for ( count = 0; count < 25; count++)
    {
        //SetPWM(7,val);
        //val = ReadAnalog(0);
        //printf("%d\n",val);
       for (pin=0; pin <8; pin++)
       {
          write_digital(pin,1);
       }
       Sleep(0,50000);
       for (pin=0; pin < 8; pin++)
       {
          write_digital(pin,0);
       }
       Sleep(0,50000);
       printf("DIG OUt:%d out of 250\n",count);
    }
