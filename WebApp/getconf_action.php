
    <?php
		date_default_timezone_set("PRC");
		
		require 'configuration.php';
		
		$con = mysqli_connect($dbcnf['addr'],$dbcnf['user'],$dbcnf['pwd'],$dbcnf['name']);
		if (!$con)
		{
			die('Could not connect mysql: ' . mysql_error());
		}

		$sql= "SELECT * FROM feeder_cnf";
		$result = mysqli_query($con,$sql);
		
		if(!empty($result)) {
			$row = mysqli_fetch_array($result);
		}else{
			$row=array();
		}
		
		mysqli_close($con);
		
		echo json_encode($row);
	?>
     


