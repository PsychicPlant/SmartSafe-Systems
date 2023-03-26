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



