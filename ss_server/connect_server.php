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
	
		/*
		 * !!!!This part is still hanging when opening the files.
		 * It must be programmed with JS to time out after a couple seconds and allow the user to retry
		 * the connection manually.
		 */
		
		//Opening connection to MYFIFO1 & 2 files, which will be used to pass 
		//data back and forth from the webserver and the application.
		
		$filename1 = "/home/gui/AVRserial/combs1";
		$filename2 = "/home/gui/AVRserial/combs2";
		
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
