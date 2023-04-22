<?php
/*
 * insertuser.php
 * 
 * Copyright 2023 root <root@raspigui>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
	<title>untitled</title>
	<meta http-equiv="content-type" content="text/html;charset=utf-8" />
	<meta name="generator" content="Geany 1.37.1" />
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
		
		$name = htmlspecialchars($_GET["name"]);
		$number = htmlspecialchars($_GET["number"]);
		$email = htmlspecialchars($_GET["email"]);
		
		$sql = "insert into Combinations (name, phone, email) values ('$name', '$number', '$email');";
		if((mysqli_query($conn, $sql)) == false)
			{echo "Failure to insert data into database";}
		
		mysqli_close($conn);
		?>
		
		<script>window.open("index.php","_self");</script>
	</p>
	
</body>

</html>
