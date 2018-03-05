
    <?php
		date_default_timezone_set("PRC");
		
		require 'configuration.php';
		
		$con = mysqli_connect($dbcnf['addr'],$dbcnf['user'],$dbcnf['pwd'],$dbcnf['name']);
		if (!$con)
		{
			die('Could not connect mysql: ' . mysqli_error());
		}
		//获取总记录数:  
		$sqls  = "SELECT COUNT(*)  as total FROM feeder_log where id  IS NOT NULL";  
		
		$sqlcount = mysqli_query($con,$sqls);  
		$pagecount  = mysqli_fetch_array($sqlcount);  
		$pagecount = $pagecount['total'];  
		mysqli_free_result($sqlcount);   //查询完记得释放空间  
		
		//获取页数,判断是否符合要求  
		$page = intval($_GET['page']);
		
		if($page <=0) {
			$page= 1;
		} elseif($page > $pagecount){ 
			$page = $pagecount; 
		}  
		  
		//定义每页显示的页数：  
		$pageshow = 10;  
		$pagesize = ($page-1) * $pageshow;  
		//计算总页数:  
		$numpages = ceil($pagecount/$pageshow);   //向上取整；  
		  
		//连接要显示的数据库  
		$sql = "SELECT * FROM feeder_log ORDER BY id desc limit $pagesize,$pageshow";  
		$sqlinfo = mysqli_query($con, $sql);  
		$result=array();
		if(!empty($sqlinfo)) {
			$divide = mysqli_fetch_array($sqlinfo, MYSQL_ASSOC); 
			while($divide){  
				$divide['info']=str_replace("<","&lt;",$divide['info']);
				$divide['info']=str_replace(">","&gt;",$divide['info']);
				$result[ ] = $divide; 
				$divide = mysqli_fetch_array($sqlinfo, MYSQL_ASSOC); 
			}  
		}
		
		mysqli_close($con);
				
		echo json_encode($result);
	?>
     


