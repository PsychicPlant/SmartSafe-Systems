<?php
/*
 * registeruser.php
 
 * 
 */

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
	<title>Registering User Combination and Fingerprint</title>
	
</head>

<body>
	<p>
	<?php
	$filename1 = "/home/gui/AVRserial/pipe1";
	$filename2 = "/home/gui/AVRserial/pipe2";
	$user = $_GET["user"];
	
	if($MYFIFO1 = fopen($filename1, "w"))
	{
		echo "</br>FIFO write pipe $filename1 opened.</br>Opening read pipe.</br>";
		
		if($MYFIFO2 = fopen($filename2, "r"))
		{
			echo "</br>FIFO read pipe $filename2 opened.</br>Commencing user registration processes...</br>";
			
			//Establishes the special chars meant for data integrity.
			$tmp = chr(0xF3);	
			
			//Begins the process of fetching the 6 digit combination and registering it to the $combination
			//variable. This url is called by index.php async with AJAX, so the 
			
			fwrite($MYFIFO1, "$tmp+$tmp", 3);
			$combination = fread($MYFIFO2, 6);
			echo json_encode($combination);
			//echo "</br>Registered combination : $answer for user $user";
			fclose($MYFIFO1);
			fclose($MYFIFO2);
		}
		else echo "Unable to open MYFIFO2 file $filename2</br>";
	}
	else echo "Unable to open MYFIFO1 file $filename1</br>";
	?>
	</p>
	<script>//window.open("index.php", "_self");</script>
</body>

</html>
