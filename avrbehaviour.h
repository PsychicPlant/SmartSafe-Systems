volatile int state = (STATE);		//Default state Input = 0; Reg = 0
char avrkey2str(int);
int comb_process(char, char *);
int avrprocessing(char *, char *);
void clear_avrbuf(void);

char avrkey2str(int key){
	
	char character = 0;
	switch(key)
	{
			case 0xcf:
				
				character = '1';
				break;
				
			case 0x9f:
			
				character = '2';
				break;
				
			case 0xdb:
			
				character = '3';
				break;
				
			case 0xee:
			
				character = '4';	
				break;
				
			case 0xbe:
			
				character = '5';
				break;
				
			case 0xfa:
			
				character = '6';
				break;
				
			case 0xed:
			
				character = '7';
				break;
				
			case 0xbd:
			
				character = '8';
				break;
				
			case 0xf9:
			
				character = '9';
				break;
				
			case 0xe7:
			
				character = '*';
				break;
				
			case 0xb7:
			
				character = '0';
				break;
				
			case 0xf3:
			
				character = '#';
				break;
				
			default:
				
				return '!';
	}
	
	return character;
}

volatile int avroutput;
int i = 0;
int b = 0;

int avrprocessing(char *bufpt, char *comb_buf){
	
	while((avroutput = (int)*(++bufpt)) != 0xF1)
	{

		if ((avroutput == 0xF3) && ((state & AVRMODE) == 0))
		{
		  printf("\nMode ON\n");
		  clear_avrbuf();
		  state = state | AVRMODE;
		}
		else if ((avroutput == 0xF3) && ((state & AVRMODE) == 1))
		{
		  printf("\nMode OFF\n");
		  state = state & ~(AVRMODE);
		}
		else if (avroutput == 0xE7)
		{
			printf("\nRe-enter combination\n");
			clear_avrbuf();
		}
		else if (avroutput == 'c')
		{
			printf("Status: Closed\n");
			printf("\nMode OFF\n");
			state = state & ~(AVRMODE);
		}
		else if (avroutput == 'o')
		{
			printf("Status: Open\n\nWarning. This safe will auto-lock in approximately 3 seconds.");
		}
		else
		{ 
			printf("%c", avrkey2str(avroutput));
			b = (comb_process(avroutput, comb_buf));
			switch(b)
			{
				case 1:
					//printf("!%s!\n", comb_buf);
				break;
				
				case 0:
					printf("\n!%s!\t>>Combination acquired successfully!\n", comb_buf);
					b = 0;
					i = 0;
					return 1;
				break;
				
				case -1:
				break;
					printf("\n!!ERROR!!\n");
				
			}
		}
	}
	
	return 0;
}

void clear_avrbuf(void)
{
	memset(comb_buf, 0, sizeof comb_buf);
	b = 0;
	i = 0;
	return;
}

int comb_process(char input, char *comb)
{

	if(i < 6 && i >= 0)
	{
		*(comb+i) = avrkey2str(input);
		if(i == 5)
		{
			return 0;
		}
	}
	else return -1;
	
	i++;
	return 1;
}

char* comb_eval(char* comb)
{
	char* user_ret = malloc(11 * sizeof (char));
	
	MYSQL *mysql = mysql_init(NULL);
    const char *host = "localhost";
    const char *user = "gui";
    const char *passwd = "gui";
    const char *db = "SmartSafe";
    unsigned int port = 0;
    const char *unix_socket = NULL;
    unsigned long client_flag = 0;
	
	mysql = mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag);
	
	char* query = calloc(128, sizeof(char));
	sprintf(query, "SELECT name FROM Combinations WHERE combination=%s;", comb);
	mysql_query(mysql, query);
	MYSQL_RES *result = mysql_store_result(mysql);
	
	MYSQL_ROW row;
	if((row = mysql_fetch_row(result)))
		user_ret = row[0];
	else 
	{
		mysql_close(mysql);
		return NULL;
	}
	if(!(mysql_fetch_row(result)))
	{	
		mysql_close(mysql);
		return user_ret;
	}
	mysql_close(mysql);
	return NULL;
}

