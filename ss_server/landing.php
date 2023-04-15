<!DOCTYPE html>
<?php
	$include_path = ".;";
?>
<html>
<head>
	<title>Landing SmartSafe Webserver Page</title>
	<script type="text/javascript">
			function connect_req()
			{
				const xhttp = new XMLHttpRequest();
				xhttp.onload = function ()
				{
					x = document.getElementById(1);
					x.innerHTML = this.response;
				}
				
				xhttp.open("GET", "connect_server.php");
				xhttp.send();
					
			}
			
	</script>
</head>

<body>
	<p>
	<?php
		$servername = "localhost";
		$username = "gui";
		$password = "gui";
		$dbname	= "SmartSafe";
	
		$conn = mysqli_connect($servername, $username, $password, $dbname);
		if(!$conn)
		{
			echo "Connection Refused.</br>";
			die("Connection failed :(\n" . mysqli_connect_error());
		}
		else echo "Connection success to database $dbname.</br>";
		echo "Connecting to SmartSafe application...</br>";
	?>
	</p>
	</br>
	<input type="button" onclick="connect_req();" value="Connect">
	</br>
	</br>
	<p id=1 name='connect_resp'></p>
</body>

</html>
