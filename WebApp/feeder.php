<?php

require 'configuration.php';

$type = $_POST['t'];
$subtype = $_POST['s'];
$info = $_POST['i'];
$serverdate=date("Y-m-d_H:i:s_w");
$curdate=date("Y-m-d H:i:s");
$resText="";

if($type==1 || ($subtype==5 && $type!=2) ) {
	//心跳消息上报的年，月，日，时，分不一致才返回对时时间
	$sendtime=1;
	if($subtype==5) {
		$newinfo=str_replace("_",":",$info);
		$newinfo=str_replace("-",":",$newinfo);
		$listItem=explode(":",$newinfo);
		if(count($listItem)!=7) {
			$sendtime=1;
		} else {
			$listTime=date_parse($curdate);
			if(intval($listItem[1])==$listTime['year'] && 
				intval($listItem[2])==$listTime['month'] &&
				intval($listItem[3])==$listTime['day'] &&
				intval($listItem[4])==$listTime['hour'] &&
				intval($listItem[5])==$listTime['minute'])
				{
					$sendtime=0;
				}
		}
	}
	if($sendtime==1) {
		$resText="<t!" . $serverdate . ">";
	} else {
		$resText="<e!" . $serverdate . ">";
	}
	echo $resText;
} else if($type==2) {
	$sql= "SELECT * FROM feeder_cnf";
	$con = mysql_connect($dbcnf['addr'],$dbcnf['user'],$dbcnf['pwd']);
	mysql_select_db($dbcnf['name'], $con);
	$result = mysql_query($sql);
	$row = mysql_fetch_array($result);
	mysql_close($con);
	if($row) {
		if($row['state']==0)
		{
			$sql= "UPDATE feeder_cnf SET state=1,acttime='{$curdate}'";
			$con = mysql_connect($dbcnf['addr'],$dbcnf['user'],$dbcnf['pwd']);
			mysql_select_db($dbcnf['name'], $con);
			$result = mysql_query($sql);
			mysql_close($con);
		}
		//这里每次都要返回配置，因为不确定设备侧的配置有没有被改写过
		$resText="<c!{$row['a1h']}:{$row['a1m']}:{$row['a1s']}_{$row['a2h']}:{$row['a2m']}:{$row['a2s']}_{$row['count']}>";
		echo $resText;
	} else {
		$resText="<e!17:10:00_20:00:00_3>";
		echo $resText;
	}
}

$con = mysql_connect($dbcnf['addr'],$dbcnf['user'],$dbcnf['pwd']);
if (!$con)
{
	die('Could not connect mysql: ' . mysql_error());
}
$info=$info."|".$resText;
mysql_select_db($dbcnf['name'], $con);
$sql="INSERT INTO feeder_log(`type`, subtype, info, recordtime) VALUES ({$type}, {$subtype}, '{$info}','{$curdate}')";

mysql_query($sql);

mysql_close($con);
?>