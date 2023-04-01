char avrkey2str(int);
int comb_process(char, char *);
int avrprocessing(char *, char *);

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
		
		if (avroutput == 0xF3)
		  printf("\nMode\n");
		else if (avroutput == 0xE7)
		  printf("\nRe-enter combination\n");
		else
		{ 
			printf("\n%c\n", avrkey2str(avroutput));
			b = (comb_process(avroutput, comb_buf));
			switch(b)
			{
				case 1:
					printf("\n!%s!\n", comb_buf);
				break;
				
				case 0:
					printf("\n!%s!\t>>Combination acquired successfully!", comb_buf);
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

int comb_process(char input, char *comb)
{

	if(i < 6 && i >= 0)
	{
		*(comb+i) = avrkey2str(input);
		if(i == 5)
		{
//			i++;
//			*(comb+i) = 0x0d;
			return 0;
		}
	}
	else return -1;
	
	i++;
	return 1;
}

