#include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <termios.h>
        #include <stdio.h>
        #include <stdlib.h>
        #include <strings.h>
        #include <string.h>
        #include <unistd.h>

        /* baudrate settings are defined in <asm/termbits.h>, which is
        included by <termios.h> */
        #define BAUDRATE B2400   

//		The BAUDRATE for our application is yet to be determined, but must start at 2400. As such, just as indicated in the termios.h file, we will set this value to B2400.
       
        /* change this definition for the correct port */
        #define MODEMDEVICE "/dev/serial0"

//		The MODEMDEVICE macro will define the device we will be opening. In my case, this device would be “/dev/ttyS0”.

        #define _POSIX_SOURCE 1 /* POSIX compliant source */

        #define FALSE 0
        #define TRUE 1

        volatile int STOP=FALSE; 

//		The volatile keyword defines a variable that will change due to external interactions. It is necessary to specify this because we do not want the compiler optimizing the code for the execution of that pre-defined var value.

      int main(void)
        {
          int fd, res;

//	fd stands for file descriptor, and it is a pointer to the open file that we will be inspecting, reading and writing to. The integer that it returns is an index number of the open file process.

          struct termios oldtio,newtio;

//		The termios structure is defined in the “asm/termbits.h” file and is used by the library to create all three models of serial com.
//		There are two termios structs being created here. One will contain the current device settings (default), before the program can do any changes to it, called oldtio. The newtio will serve to configure the new terminal connection.

          char buf[255];
        /* 
          Open modem device for reading and writing and not as controlling tty
          because we don't want to get killed if linenoise sends CTRL-C.
        */
         fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 

//		This opens the file with R/D permission & more, and returns an int value that will be stored in the fd variable.

         if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
//		This will return an error in case the file could not be opened. It will then quit out of the program with the exit code -1.

         tcgetattr(fd,&oldtio); /* save current serial port settings */

//		The tcgetattr function will get the terminal parameters of the file opened, described by fd, and save it into a termios struct. This is where we will use our oldtio.

         bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
        


//		The bzero function erases all bytes the sizeof specified object, starting at a memory location.








        /* 
          BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
          CRTSCTS : output hardware flow control (only used if the cable has
                    all necessary lines. See sect. 7 of Serial-HOWTO)
          CS8     : 8n1 (8bit,no parity,1 stopbit)
          CLOCAL  : local connection, no modem contol
          CREAD   : enable receiving characters
        */
         newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;

//		There is no necessity for the CRTSCTS flag in our configuration, since we will not be using those communication pins.

//PARENB is a value that will set the Even Parity. It will generate that parity on write and inspect it on read.

         
        /*
          IGNPAR  : ignore bytes with parity errors
          ICRNL   : map CR to NL (otherwise a CR input on the other computer
                    will not terminate input)
          otherwise make device raw (no other input processing)
        */
         newtio.c_iflag = IGNPAR | ICRNL;
         
//		We would like to have parity in our system, and as such instead of IGNPAR, we will use INPCK.


        /*
         Raw output.
        */
         newtio.c_oflag = 0;
         
        /*
          ICANON  : enable canonical input
          disable all echo functionality, and don't send signals to calling program
        */
         newtio.c_lflag = ICANON;

//		For testing, we will have the ECHO flag here.
//		This is an example of a Canonical model. In our Non-canonical, we will set this to 0.

        /* 
          initialize all control characters 
          default values can be found in /usr/include/termios.h, and are given
          in the comments, but we don't need them here
        */
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
        
        /* 
          now clean the modem line and activate the settings for the port
        */
         tcflush(fd, TCIFLUSH);
         tcsetattr(fd,TCSANOW,&newtio);
        
        /*
          terminal settings done, now handle input
          In this example, inputting a 'z' at the beginning of a line will 
          exit the program.
        */
         while (STOP==FALSE) {     /* loop until we have a terminating condition */
         /* read blocks program execution until a line terminating character is 
            input, even if more than 255 chars are input. If the number
            of characters read is smaller than the number of chars available,
            subsequent reads will return the remaining chars. res will be set
            to the actual number of characters actually read */
            res = read(fd,buf,255); 
            buf[res]=0;             /* set end of string, so we can printf */
            if (strcmp(buf, "F3") == 0)
              printf("Mode Switch\n");
            printf(":%s", buf);
            if (buf[0]=='z') STOP=TRUE;
         }
         /* restore the old port settings */
         tcsetattr(fd,TCSANOW,&oldtio);
        }
