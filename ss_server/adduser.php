<html>
<head>
	<title>SmartSafe New User</title>
</head>
<body>
	<h3>Add SmartSafe User</h3>
	</br>
	<p>Use the below form to add a new user to the SmartSafe database.</p>
	<form action="insertuser.php" method="get" target="_self">
		<p align="justify">
		<fieldset>
		    <legend>Name</legend>
				<input type="text" id="name" name="name" size="15" min="0" required>
	    </fieldset>
		</br> 
	    <fieldset>
	    <legend>Contact Information</legend>
			<label for="number">Phone Number:</label>
			<input type="text" id="number" name="number" size="10" required>
		
			<label for="earth">Email Address(Optional):</label>
			<input type="text" id="email" name="email" size="15" >
			</br>
		</fieldset>
		</br>
		<input type="submit" value="Submit!">
	</form>
</body>
</html>
