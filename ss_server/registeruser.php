<?php

//This url is called by index.php async with AJAX.

header("Content-Type: application/json");

$servername = "localhost";
$username = "gui";
$password = "gui";
$dbname	= "SmartSafe";

$conn = mysqli_connect($servername, $username, $password, $dbname);

$filename1 = "/home/gui/AVRserial/pipe1";
$filename2 = "/home/gui/AVRserial/pipe2";
$user = urldecode($_GET["user"]);

if($MYFIFO1 = fopen($filename1, "w"))
{
	if($MYFIFO2 = fopen($filename2, "r"))
	{
		//Establishes the special chars meant for data integrity.
		$tmp = chr(0xF3);	
		
		//Begins the process of fetching the 6 digit combination and registering it to the $combination
		//variable.
		//Sends webserver code 0xF3 + 0xF3
		
		fwrite($MYFIFO1, "$tmp+$tmp", 3);
		$combination = fread($MYFIFO2, 6);
		$arr = array('comb' => $combination);
		
		//Begins the process of fetching the fingerprint file name and registering it to the $fingerprint
		//variable.
		//Sends webserver code 0xF3 @ 0xF3
		/*
		fwrite($MYFIFO1, "$tmp@$tmp", 3);
		$fingerprint = fread($MYFIFO2, 6);
		$arr = array('finger' => $fingerprint);
		*/
		echo json_encode($arr);
		//echo "$user : $combination";
		
		$sql = "update Combinations set combination='$combination' where name='$user';";
		$result = mysqli_query($conn, $sql);

		mysqli_close($conn);
		fclose($MYFIFO1);
		fclose($MYFIFO2);
	}
}
?>

