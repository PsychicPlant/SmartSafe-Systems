#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define STATE 0x00
#define AVRMODE 0x01    //Defines if AVR is in input mode or not
#define AVRREG 0x02     //Defines if AVR is registering a new user, or evaluating an existing one
#include </home/gui/AVRserial/avrbehaviour.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B2400 
#define MYFIFO1 "pipe1"   //Used to read from different devices
#define MYFIFO2 "pipe2"   //Used to write to different devices


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
    
    /*
     *  fd1 is going to be a file descriptor for serial0, where it will be used to read/write from/to it.
     *  res is the result variable when reading the contents of serial0. Contains number of characters read.
     *  avrresp is the return value of avrprocessing() -> avrbehaviour.h function, which will determine AVR input processing behaviour.
     *      currently used to check if 6 input keys have been pressed before writting digits to a file.
    */
    
    struct termios oldtio,newtio;   //These structs are important for starting, configuring, using and quitting a new terminal application.
    char avr_buf[512];              //Used to store AVR serial information like debug strings or input read from fd1
    char cli_buf[512];              //Used to store stdio input read from fd STDIN_FILENO
    char child_buf[512];            //Used to store child input read from fd read_child (pipe)
    char fifo_buf[512];             //Used to store fifo input read from fd fifofd_r (pipe)
    char comb_buf[7] = {0};         //Used to store AVR key input (determined by values in avr_buf)
    pid_t cpid;                     //This variable will store the state of the fork() function
    int pipefd[2];                  //The following are file descriptors for the different pipes
    int pipe_stdio_1[2];            
    int pipe_stdio_2[2];

    umask(011);
    if(access(MYFIFO1, F_OK) != 0)   //This statement checks the existance of a fifo file of name defined by MYFIFO1, and promotes its creation
    {                               //if it does not exist.
        if(mkfifo(MYFIFO1, S_IRWXO | S_IRWXG | S_IRWXU | 0777) == -1)
          {perror("Error making FIFO file."); exit(-1);}
        else
          printf("Making new FIFO file: %s\n", MYFIFO1);
    }
    
    if(access(MYFIFO2, F_OK) != 0)   //This statement checks the existance of a fifo file of name defined by MYFIFO1, and promotes its creation
    {                               //if it does not exist.
        if(mkfifo(MYFIFO2, S_IRWXO | S_IRWXG | S_IRWXU | 0777) == -1)
          {perror("Error making FIFO file."); exit(-1);}
        else
          printf("Making new FIFO file: %s\n", MYFIFO2);
    }

    //The following statements will create pipes using the file descriptor arrays provided.
    //These are done right before the forking of this program.
    pipe(pipefd);
    pipe(pipe_stdio_1);
    pipe(pipe_stdio_2);
    
    if(fork() == 0)
    {
        system("curl -s http://localhost/connect_server.php");
        exit(0);
    }
    else

    cpid = fork();              //This statement creates a copy of this process and its memory (see fork() for more information) called a child process.
    if (cpid == -1) {           //Indicates any failure in creating a child process.
        perror("fork");         
        exit(EXIT_FAILURE);
    }
    /*
     *  If the process is the child, it will perform the following instructions.
     *  The goal of doing this is to create a separate process that runs in parallel with this terminal application.
     *  Since this program needs to be very dynamic, taking into account different sources of input and output, I found it easier to separate these
     *    tasks. 
     *  One big contributor to this is also the fact that I will be receiving signals from the webserver, and those i would like to be processed 
     *    in parallel with my main application.
    */
    
    
    if (cpid == 0)              
    {  

        #include </home/gui/AVRserial/childbehaviour.h>
        exit(0);
    } 

    close(pipefd[0]);  
    close(pipe_stdio_1[0]); //closes reading end in stdio1
    close(pipe_stdio_2[1]); //closes writting end in stdio2   
        
    int write_child = pipe_stdio_1[1];
    int read_child = pipe_stdio_2[0];
    int cmd_child_write = pipefd[1];
    
    struct pollfd *pfds;            //This structure is necessary for the functioning of the poll() function.
    int numfd = 4;                  //This variable holds the number of file descriptors that will be polled at once.
    pfds = calloc(numfd, sizeof(struct pollfd));    //This struct will contain all fds to be polled, and their expected behavior.
 

    fd1 = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    pfds[0].fd = fd1;
    pfds[0].events = POLLIN;
    
    pfds[1].fd = STDOUT_FILENO;
    pfds[1].events = POLLIN;
    
    pfds[2].fd = read_child;
    pfds[2].events = POLLIN;
    
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

    
    while (STOP==FALSE) 
    {     
    /* loop until we have a terminating condition */
    /* read blocks program execution until a line terminating character is 
    input, even if more than 255 chars are input. If the number
    of characters read is smaller than the number of chars available,
    subsequent reads will return the remaining chars. res will be set
    to the actual number of characters actually read */

        
        int ready = poll(pfds, numfd, 10);
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
                //printf("POLLIN\n");
                res = read(fd1,avr_buf,255); 
                avr_buf[res]= '\0';             /* set end of string, so we can printf */
                char *bufpt = &avr_buf[0];
                
                if (*bufpt == 0xF1)
                {
                    avrresp = avrprocessing(bufpt, comb_buf);
                    if(avrresp == 1)
                    {
                            //This checks if the AVRREG flag was set.
                            //If it was, it will send the combination thru the pipe towards
                            //the webserver using 0xF3 as an integrity check. The combination will still
                            //be logged into the COMBTXT file.
                            
                          if(state == (STATE | AVRMODE | AVRREG))   
                          {

                                //Sends reg_status to child, which sends to webserver
                                dprintf(cmd_child_write, "reg_ok\n");
                                dprintf(write_child, "%s", comb_buf);
                                
                                //Returns input state to default STATE
                                state = STATE;
                                printf("Registered user combination!\n");
                          }
                              
                          FILE * fd2 = fopen(COMBTXT, "a" ); 
                          if (fd2 <0) {perror(COMBTXT); exit(-1); }
                            
                          fprintf(fd2, "%s\n", comb_buf);
                          fclose(fd2);
                          memset(comb_buf, 0, sizeof comb_buf);
                    }
                    
                }
                else printf("%s", avr_buf);
            }
            else if(pfds[1].revents & POLLIN)
            {
                  
                int r = read(STDIN_FILENO, &cli_buf, 512);
                cli_buf[r] = '\0';
                //printf("%s", cli_buf);
                
                if(strcmp(cli_buf, "q\n") == 0)
                {
                    printf("Terminating session. Stand by.\n");
                    int child_status = 0;
    
                    dprintf(cmd_child_write, "exit\n");
                    
                    wait(&child_status);
                    if(WIFEXITED(child_status))
                      printf("Child successfully killed!\n");
                    else
                      printf("Error killing this child!\n");
                      
                    STOP = TRUE;
                    
                }
                else if(strcmp(cli_buf, "t\n") == 0)
                {
                    printf("Writting to child cmd.\n");
                    dprintf(cmd_child_write, "Command\n");
                    
                    printf("Writting to child text.\n");
                    dprintf(write_child, "Text\n");
                }
                else if(strcmp(cli_buf, "o\n") == 0)
                {
                    dprintf(fd1, "%c%c%c", 0xF2, 'o', 0xF2);
                    printf("Open command sent!\n");
                }
                else if(strcmp(cli_buf, "s\n") == 0)
                {
                    dprintf(fd1, "%c%c%c", 0xF2, 's', 0xF2);
                    printf("Status command sent!\n");
                }
                else if(strcmp(cli_buf, "c\n") == 0)
                {
                    dprintf(fd1, "%c%c%c", 0xF2, 'c', 0xF2);
                    printf("Close command sent!\n");
                }
            }
            else if(pfds[2].revents & POLLIN)
            {
                int r = read(read_child, &child_buf, 512);
                child_buf[r] = '\0';
                
                     //Checks for "+\n" cmd sent from the webserver to begin registration
                     //operations. This will set AVRMODE and AVRREG flags which will async enable the 
                     //keypad to work (without needing to press #) and will change the interpretation of
                     //the fetched combination in the pfds[0] events above.
                
                if(strcmp(child_buf, "+\n") == 0)          
                {   
                    //Begins async input from the AVR
                    printf("Detected\n");
                    dprintf(fd1, "%c%c%c", 0xF2, 'r', 0xF2);
                    
                    //Changes state variable to indicate AVR is in input mode
                    //state = 0b00000011
                    state = (STATE | AVRMODE | AVRREG); 
                              
                }
                else
                    printf("%s", child_buf);
            }

            
            printf("\n");
            break;
          }
          

        }
    
    }
    /* restore the old port settings */
    
    
    for(int i = 0; i < (numfd + 1); i++)
    {
        close(pfds[i].fd);
    }
    
    free(pfds);
    tcsetattr(fd1,TCSANOW,&oldtio);
    return 0;
}

int test(void)
{

  return 0;
}
  
