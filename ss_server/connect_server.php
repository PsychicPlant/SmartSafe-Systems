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
				$tmp = chr(0xF3);	
				fwrite($MYFIFO1, "$tmp!$tmp", 3);
				$answer = fread($MYFIFO2, 4);
				echo "</br>$answer";
				
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
