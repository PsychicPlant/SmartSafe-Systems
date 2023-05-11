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
				connect_to = setTimeout(function (){connect_refused(); xhttp.abort();}, 3000);
				
				const xhttp = new XMLHttpRequest();
				xhttp.onload = function ()
				{
					clearTimeout(connect_to);
					
					x = document.getElementById(0);
					y = document.getElementById(1);
					z = document.getElementById(2);
					a = document.getElementById(3);
					document.getElementById('select').style = true;
					z.disabled = false;
					a.disabled = false;
					y.disabled = true;
					
					x.innerHTML = this.responseText;
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
				xhttp.overrideMimeType("application/json");
				xhttp.onload = function()
					{
					
						let objJSON = JSON.parse(this.responseText);
						x = objJSON.comb;
						document.getElementById(0).innerHTML = x;
						document.getElementById(0).disabled = false;
					
					};
					
				var user_encoded = encodeURIComponent(user);
				//window.alert(user);
				xhttp.open("GET", "registeruser.php?user=" + user_encoded, true);
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
	<?php
		$sql = "select id, name from Combinations;";
		$result = mysqli_query($conn, $sql);
		echo "<select id='select' style='display: none'>";
		foreach($result as $row)
		{
			echo "<option value=\"{$row['name']}\">{$row['name']}</option>";
		}
		echo "</select>";
	?>
	</br>
	</br>
	<input id=3 type="button" onclick="registeruser(document.getElementById('select').value);" value="Register User" disabled>
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
