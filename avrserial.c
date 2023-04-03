#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include </home/gui/AVRserial/avrbehaviour.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B2400 
#define MYFIFO "combs" 

//		The BAUDRATE for our application is yet to be determined, but must start at 2400. As such, just as indicated in the termios.h file, we will set this value to B2400.

/* change this definition for the correct port */
#define MODEMDEVICE "/dev/serial0"
#define COMBTXT "/home/gui/AVRserial/combinations"

//		The MODEMDEVICE macro will define the device we will be opening. In my case, this device would be “/dev/ttyS0”.

#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

//		The volatile keyword defines a variable that will change due to external interactions. It is necessary to specify this because we do not want the compiler optimizing the code for the execution of that pre-defined var value.

int main(void)
{
    
    int fd1, res, avrresp;
    struct termios oldtio,newtio;
    char buf[512];
    char cli_buf[512];
    char comb_buf[7] = {0};
    pid_t cpid;
    int pipefd[2];
    int pipe_stdio_1[2];
    int pipe_stdio_2[2];

    struct pollfd *pfds;
    pfds = malloc(sizeof(struct pollfd));
 

    if(access(MYFIFO, F_OK) != 0)
    {
        if(mkfifo(MYFIFO, 0666) == -1)
          {perror("Error making FIFO file."); exit(-1);}
    }

    pipe(pipefd);
    pipe(pipe_stdio_1);
    pipe(pipe_stdio_2);
    
    //int write_child = pipe_stdio_1[1];
    int read_child = pipe_stdio_2[0];
    int cmd_child_write = pipefd[1];
    

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0)
    {  
        close(pipefd[1]);
        close(pipe_stdio_1[1]); //closes writting end in stdio1
        close(pipe_stdio_2[0]); //closes reading end in stdio2
        dup2(pipe_stdio_1[0], STDIN_FILENO);
        dup2(pipe_stdio_2[1], STDOUT_FILENO);
        
        struct pollfd *child_pfds;
        child_pfds = malloc(sizeof(struct pollfd));
        int write_parent = STDOUT_FILENO;
        //int read_parent = STDIN_FILENO;
        int cmd_child_read = pipefd[0];
        char child_cmd[255];

        
        child_pfds[0].fd = cmd_child_read;
        child_pfds[0].events = POLLIN;

        //char ping[] = "PONG\n";
        //write(STDOUT_FILENO, &ping, sizeof(ping));

        int fifofd_w = open(MYFIFO, O_WRONLY);
        if (fifofd_w == -1)
          {perror("Error opening FIFO file for writting."); exit(-1);}
        else
          dprintf(write_parent, "Opened file for writting on avrchild.\n");

        int stop = TRUE;
        while(stop)
        {
            int prob = poll(child_pfds, 1, 10);
            switch(prob)
            {
                case -1:
                    dprintf(write_parent, "Error polling.\n");
                    exit(-1);
                    break;
                case 0:
                    break;
                
                default:
                    if(child_pfds[0].revents & POLLIN)
                    {
                        read(cmd_child_read, &child_cmd, 255);
                        dprintf(write_parent, "%s!!", child_cmd);
                        if(strcmp(child_cmd, "exit\n") == 0)
                          {
                              dprintf(write_parent, "Killing child. Stand by.\n");
                              exit(0);
                          }
                    }
            }
        }
        /* 
        char** pipe_arg = malloc(sizeof(*pipe_arg));
        sprintf(pipe_arg[0],"%d", pipefd[0]);
        pipe_arg[1] = NULL; 
        execv("avrcli", pipe_arg);
        perror("ERROR executing program avrcli.");
        */
        exit(-1);
        
    } 
    else 
    {
        close(pipefd[0]);  
        close(pipe_stdio_1[0]); //closes reading end in stdio1
        close(pipe_stdio_2[1]); //closes writting end in stdio2   
  
        int fifofd_r = open(MYFIFO, O_RDONLY);
        if (fifofd_r == -1)
          {perror("Error opening FIFO file for reading."); exit(-1);}
        else
          printf("Opened file for reading on avrserial.\n");
    }

    
          
    char cool[512];
    int g = read(read_child, &cool, sizeof(cool));
    cool[g] = '\0';
    printf("%s\n", cool);
         

    fd1 = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    pfds[0].fd = fd1;
    pfds[0].events = POLLIN;
    
    pfds[1].fd = STDOUT_FILENO;
    pfds[1].events = POLLIN;
    
    if (fd1 <0) {perror(MODEMDEVICE); exit(-1); }
    
    tcgetattr(fd1,&oldtio); /* save current serial port settings */
    
    bzero(&newtio, sizeof(newtio)); 
    
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */
    
    tcflush(fd1, TCIFLUSH);
    tcsetattr(fd1,TCSANOW,&newtio);

    
    while (STOP==FALSE) {     
    /* loop until we have a terminating condition */
    /* read blocks program execution until a line terminating character is 
    input, even if more than 255 chars are input. If the number
    of characters read is smaller than the number of chars available,
    subsequent reads will return the remaining chars. res will be set
    to the actual number of characters actually read */

    int ready = poll(pfds, 2, 100);
    switch(ready)
    {
      case -1:
        printf("Error polling.\n");
        exit(-1);
        break;
      case 0:
        break;
      
      default:
      {
        if(pfds[0].revents & POLLIN)
        { 
            printf("POLLIN\n");
            res = read(fd1,buf,255); 
            buf[res]=0;             /* set end of string, so we can printf */
            char *bufpt = &buf[0];
            
            
            if (*bufpt == 0xF1)
            {
              avrresp = avrprocessing(bufpt, comb_buf);
              if(avrresp == 1)
              {
                FILE * fd2 = fopen(COMBTXT, "a" ); 
                if (fd2 <0) {perror(COMBTXT); exit(-1); }
                
                fprintf(fd2, "%s\n", comb_buf);
                fclose(fd2);
                memset(comb_buf, 0, sizeof comb_buf);
              }
            }
          }
          else if(pfds[1].revents & POLLIN)
          {
              
              int r = read(STDIN_FILENO, &cli_buf, 512);
              cli_buf[r] = '\0';
              printf("%s", cli_buf);
              
              if(strcmp(cli_buf, "q\n") == 0)
                {
                  printf("Terminating session. Stand by.\n");
                  STOP = TRUE;
                }
              else if(strcmp(cli_buf, "t\n") == 0)
              {
                  dprintf(cmd_child_write, "Ohio\n");
              }
          }
          else printf("%s", buf);
          break;
        }
      
      //if (buf[0]=='z') STOP=TRUE;
      }
    }
    /* restore the old port settings */
    
    dprintf(cmd_child_write, "exit\n");
    tcsetattr(fd1,TCSANOW,&oldtio);
    close(fd1);
    free(pfds);
    
    return 0;
}
  
