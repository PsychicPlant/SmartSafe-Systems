#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include </home/gui/AVRserial/avrbehaviour.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B2400   

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
    char buf[255];
    char comb_buf[7] = {0};
/*
    pid_t cpid;
    int pipefd[2];

    if (pipe(pipefd) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0)
    {    // Child reads from pipe 
        close(pipefd[1]);         

    } 
    else 
    {
        close(pipefd[0]);      
        
    }
*/

    fd1 = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    
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
    else printf("%s", buf);
    
    //if (buf[0]=='z') STOP=TRUE;
    }
    /* restore the old port settings */
    tcsetattr(fd1,TCSANOW,&oldtio);
}
  
