close(pipefd[1]);       //Closes writing end in childfd
close(pipe_stdio_1[1]); //Closes writing end in stdio1
close(pipe_stdio_2[0]); //Closes reading end in stdio2

struct pollfd *child_pfds;  //This is a necessary variable when polling open files and pipes through the poll() -> poll.h function.
int childnumfd = 3;         //This is the number of files that will be polled at once.
child_pfds = calloc(childnumfd, sizeof(struct pollfd));   //Allocates memory space for those structs.


int write_parent = pipe_stdio_2[1];
int read_parent = pipe_stdio_1[0];
int cmd_child_read = pipefd[0];
char child_cmd[512];
char child_text[512];


child_pfds[0].fd = cmd_child_read;
child_pfds[0].events = POLLIN;

child_pfds[1].fd = read_parent;
child_pfds[1].events = POLLIN;


int fifofd_r = open(MYFIFO1, O_RDONLY);
if (fifofd_r == -1)
  {perror("Error opening FIFO file for writting."); exit(-1);}
else
  dprintf(write_parent, "Listening for Webserver...\n");
  
  
int fifofd_w = open(MYFIFO2, O_WRONLY);
if (fifofd_w == -1)
  {perror("Error opening FIFO file for writting."); exit(-1);}
else
  dprintf(write_parent, "Connected to Webserver!\n");

child_pfds[2].fd = fifofd_r;
child_pfds[2].events = POLLIN;

//child_pfds[2].fd = fifofd_w;
//child_pfds[2].events = POLLOUT;

        
int stop = TRUE;
        while(stop)
        {
            int prob = poll(child_pfds, childnumfd, 10);
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
                        int f = read(cmd_child_read, &child_cmd, 255);
                        child_cmd[f] = '\0';
                        dprintf(write_parent, "%s", child_cmd);
                        if(strcmp(child_cmd, "exit\n") == 0)
                          {                              
                              dprintf(write_parent, "Child is exiting...\n");
                              for(int i = 0; i < (childnumfd + 1); i++)
                              {
                                  close(child_pfds[i].fd);
                              }

                              free(child_pfds);
                              stop = FALSE;
                              exit(0);
                          }
                          
                        //Mesage returned to webserver if combination was acquired and
                        //is ready to use.
                        else if(strcmp(child_cmd, "reg_ok\n") == 0)     
                        {
                            char comb_send[] = {0};
                            read(read_parent, &comb_send, 512);
                            dprintf(fifofd_w, comb_send);
                        }
                        
                        //Message returned to webserver if error occured
                        else if(strcmp(child_cmd, "reg_err\n") == 0)
                        {}
                    }
                    else if(child_pfds[1].revents & POLLIN)
                    {
                        int f = read(read_parent, &child_text, 512);
                        child_text[f] = '\0';
                        dprintf(write_parent, "%s", child_text);
                    }
                    
                    else if(child_pfds[2].revents & POLLIN)     //Reads commands sent from Webserver
                    {
		                read(fifofd_r, &fifo_buf, 512);
		                if(*fifo_buf == 0xF3)
		                {
                            //The switch case statements below serve as a lookup table for commands
                            //sent specifically by the Webserver.
		                    switch(fifo_buf[1])
		                    {
                                //The webserver sends the character encapsulated "!" character
                                //if it succeeds in opening the FIFOs.
		                        case '!':
                                    dprintf(write_parent, "Read by child\n");
                                    write(fifofd_w, "OK!", 4);
                                    dprintf(write_parent, "Wrote to server\n");
                                    break;
                                
                                //The webserver sends this signal to indicate the registration/update
                                //of a user. The command is acknowledged and sent to the parent.
                                case '+':
                                    dprintf(write_parent, "Registering user metrics...\n");
                                    sleep(1);
                                    dprintf(write_parent, "+\n");
                                    break;
                                    
		                        default:
		                          break;
		                    }
		                }
		                //fifo_buf[r] = '\0';
		                //printf("%s\n", fifo_buf);
		            }
						
						
                    break;
            }
        }
