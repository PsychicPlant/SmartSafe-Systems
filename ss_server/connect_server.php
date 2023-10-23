<!DOCTYPE html>
<?php
	$include_path = ".;";
?>
<html>
<head>
	<title>Server Connection Script</title>
</head>

<body>
	<p>
	<?php
	
		
		//Opening connection to MYFIFO1 & 2 files, which will be used to pass 
		//data back and forth from the webserver and the application.
		//This exists as a test of the system, before allowing the user to go forward
		//with the user management processes since they all depend on the named-pipe's existance.
		
		$filename1 = "/home/gui/AVRserial/pipe1";
		$filename2 = "/home/gui/AVRserial/pipe2";
		
		if($MYFIFO1 = fopen($filename1, "w"))
		{
			echo "</br>FIFO write pipe $filename1 opened.</br>Opening read pipe.</br>";
			
			if($MYFIFO2 = fopen($filename2, "r"))
			{
				echo "</br>FIFO read pipe $filename2 opened.</br>Waiting for application response...</br>";
				//------------------------------
				//The following statements will send an encapsulated "!" character to the listening child
				//which has just succeeded in opening the pipes. This will indicate to the child that they 
				//can begin communications.
				
				$tmp = chr(0xF3);	
				fwrite($MYFIFO1, "$tmp!$tmp", 3);
				
				//------------------------------
				//The script will then wait for an answer from the child. This answer is mostly symbolic,
				//as it is not parsed in this script, but it is nonetheless necessary and will cause
				//the script to hang unless it is properly done.
				//This reply is automatically programmed to be sent by the child and is defined in 
				//childbehaviour.h.
				
				$answer = fread($MYFIFO2, 4);
				echo "</br>$answer";
				
				//------------------------------
				
				fclose($MYFIFO1);
				fclose($MYFIFO2);
			}
			else echo "Unable to open MYFIFO2 file $filename2</br>";
		}
		else echo "Unable to open MYFIFO1 file $filename1</br>";
		
	?>
	</p>
</body>

</html>
