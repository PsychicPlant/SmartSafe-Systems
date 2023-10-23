volatile int state = (STATE);		//Default state Input = 0; Reg = 0
char avrkey2str(int);
int comb_process(char, char *);
int avrprocessing(char *, char *);
void clear_avrbuf(void);

const int HASH = 0xF6;
const int S1 = 0xFB;
const int S2 = 0xFD;
const int S3 = 0xFE;
const int NUM1 = 0xBB;
const int NUM2 = 0xBD;
const int NUM3 = 0xBE;
const int NUM4 = 0xDB;
const int NUM5 = 0xDD;
const int NUM6 = 0xDE;
const int NUM7 = 0xEB;
const int NUM8 = 0xED;
const int NUM9 = 0xEE;
const int POUND = 0xF3;
const int NUM0 = 0xF5;


char avrkey2str(int key){
	
	char character = 0;
	switch(key)
	{
			case 0xbb:
				
				character = '1';
				break;
				
			case 0xbd:
			
				character = '2';
				break;
				
			case 0xbe:
			
				character = '3';
				break;
				
			case 0xdb:
			
				character = '4';	
				break;
				
			case 0xdd:
			
				character = '5';
				break;
				
			case 0xde:
			
				character = '6';
				break;
				
			case 0xeb:
			
				character = '7';
				break;
				
			case 0xed:
			
				character = '8';
				break;
				
			case 0xee:
			
				character = '9';
				break;
				
			case 0xf3:
			
				character = '*';
				break;
				
			case 0xf5:
			
				character = '0';
				break;
				
			case 0xf6:
			
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

		//printf("%x\n", avroutput);
		if ((avroutput == HASH) && ((state & AVRMODE) == 0))
		{
		  printf("\nMode ON\n");
		  clear_avrbuf();
		  state = state | AVRMODE;
		}
		else if ((avroutput == HASH) && ((state & AVRMODE) == 1))
		{
		  printf("\nMode OFF\n");
		  state = state & ~(AVRMODE);
		}
		else if (avroutput == POUND)
		{
			printf("\nRe-enter combination\n");
			clear_avrbuf();
			dprintf(fd1, "%c%c%c", 0xF2, 'i', 0xF2);
		}
		else if (avroutput == 'c')
		{
			printf("Status: Closed\n");
			printf("\nMode OFF\n");
			state = state & ~(AVRMODE);
		}
		else if (avroutput == 'h')
		{
			dprintf(fd1, "%c%c%c", 0xF2, 'h', 0xF2);
			//printf(".");
		}
		else if (avroutput == 'o')
		{
			printf("Status: Open\n\nWarning. This safe will auto-lock in approximately 3 seconds.");
		}
		else if (avroutput == 'f')
		{
			printf("FUCK\n");
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

int comb_eval_id(char* name)
{
	int user_ret = 0;
	
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
	sprintf(query, "SELECT id FROM Combinations WHERE name=\"%s\";", name);
	mysql_query(mysql, query);
	MYSQL_RES *result = mysql_store_result(mysql);
	
	MYSQL_ROW row;
	if((row = mysql_fetch_row(result)))
		user_ret = atoi(row[0]);
	else 
	{
		mysql_close(mysql);
		return 0;
	}
	if(!(mysql_fetch_row(result)))
	{	
		mysql_close(mysql);
		return user_ret;
	}
	mysql_close(mysql);
	return 0;
}

