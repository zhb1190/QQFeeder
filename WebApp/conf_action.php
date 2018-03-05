
<?php
	date_default_timezone_set("PRC");

	$a1h =  intval($_POST['a1h'],10);
	$a1m =  intval($_POST['a1m'],10);
	$a1s =  intval($_POST['a1s'],10);
	$a2h =  intval($_POST['a2h'],10);
	$a2m =  intval($_POST['a2m'],10);
	$a2s =  intval($_POST['a2s'],10);
	$count = intval($_POST['count'],10);
	
	
		
	require 'configuration.php';
	
	$con = mysqli_connect($dbcnf['addr'],$dbcnf['user'],$dbcnf['pwd'],$dbcnf['name']);
	if (!$con)
	{
		die('Could not connect mysql: ' . mysql_error());
	}

	$sql= "DELETE FROM feeder_cnf";
	mysqli_query($con, $sql);
	
	$curdate = date("Y-m-d H:i:s");
	$sql = "INSERT INTO feeder_cnf(a1h,a1m,a1s,a2h,a2m,a2s,`count`,state,acttime) VALUES({$a1h},{$a1m},{$a1s},{$a2h},{$a2m},{$a2s},{$count},0,'{$curdate}')";
		
	mysqli_query($con, $sql);
	mysqli_close($con);
	
	//header("Location: ./setting.html");
	echo json_encode($_POST);
	
?>
