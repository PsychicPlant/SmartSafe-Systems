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
				connect_to = setTimeout(function (){connect_refused(); exit();}, 3000);
				
				const xhttp = new XMLHttpRequest();
				xhttp.onload = function ()
				{
					clearTimeout(connect_to);
					x = document.getElementById(0);
					y = document.getElementById(1);
					z = document.getElementById(2);
					a = document.getElementById(3);
					z.disabled = false;
					a.disabled = false;
					y.disabled = true;
					x.innerHTML = this.response;
				}
				
				xhttp.open("GET", "connect_server.php");
				xhttp.send();
					
			}
			
			function connect_refused()
			{
				x = document.getElementById(0);
				x.innerHTML = "!Connection Timed Out!</br></br>Be sure to start SmartSafe application before attempting to connect again...</br>";
			}
			
			
			function adduser()
			{
				window.open("adduser.php", "_self");
			}
			
			function registeruser(user)
			{
				
				document.getElementById(0).innerHTML = "LOADING";
				document.getElementById(3).disabled = true;
				
				const xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function()
					{
						if (this.readyState == 4 && this.status == 200) 
						{
							var print = this.responseText + "</br>" + JSON.parse(this.);
							document.getElementById(0).innerHTML = this.responseText;
							document.getElementById(0).disabled = false;
							
						}
					};
					
				xhttp.open("GET", "registeruser.php?user=" + user, true);
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
	<input id=1 type="button" onclick="connect_req();" value="Connect">
	</br>
	</br>
	<input id=2 type="button" onclick="adduser();" value="Add User" disabled>
	</br>
	</br>
	<input id=3 type="button" onclick="registeruser('user');" value="Register User" disabled>
	</br>
	</br>
	<p id=0 name='connect_resp'></p>
	<p id="usertable">
	<?php
		$sql = "select id, name, phone, email, combination from Combinations;";
		$result = mysqli_query($conn, $sql);
		echo "\n<h3>As it stands, these are the registered users:</h3>";
		echo "<p align='justify'>";
		echo "<table style='width:100%'>";
		echo "<tr> <th>ID</th> <th>Name</th> <th>Phone</th> <th>Email</th>
		 <th>Combination</th> </tr>";
		foreach($result as $row){
			echo "<tr> <td>{$row['id']}</td> <td>{$row['name']}</td> <td>{$row['phone']}</td>
			 <td>{$row['email']}</td> <td>{$row['combination']}</td></tr>";
		}
		echo "</table>";
		echo "</p>";
		
		mysqli_close($conn);
	?>
	</p>
	
</body>

</html>
