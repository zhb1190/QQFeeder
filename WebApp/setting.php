<div class="pages">
  <div data-page="projects" class="page no-toolbar no-navbar">
    <div id="setdlg" class="page-content pull-to-refresh-content">
	<div class="pull-to-refresh-layer">
    <div class="preloader"></div>
    <div class="pull-to-refresh-arrow"></div>
  </div>
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
		
		$row = mysqli_fetch_array($result);
		
		mysqli_close($con);
	?>
     <div class="navbarpages">
       <div class="navbar_home_link"><a href="index.html"><img src="images/icons/white/home.png" alt="" title="" /></a></div>
       <div class="navbar_page_center">参数设置(下拉刷新数据)</div>
       <div class="menu_open_icon_white"><a href="#" class="open-panel"><img src="images/menu_open.png" alt="" title="" /></a></div>
     </div>
     
     <div id="pages_maincontent">
      
      <h2 class="page_subtitle">参数</h2>
	  <div class="content-block">
            <div class="contactform">
            <form id="SettingForm" action="conf_action.php" method="POST" class="ajax-submit">
			<div class="formbox">
				<label>闹钟1:</label>
			</div>
			<div class="formbox">
				<label> </label>
				<input class="form_input required" type="text" id="a1h" name="a1h" size="2" maxlength="2" value="<?php echo $row['a1h'] ?>"/><label1>:</label1>
				<input class="form_input required" type="text" id="a1m" name="a1m" size="2" maxlength="2" value="<?php echo $row['a1m'] ?>"/><label1>:</label1>
				<input class="form_input required" type="text" id="a1s" name="a1s" size="2" maxlength="2" value="<?php echo $row['a1s'] ?>"/>
			</div>
			<div class="formbox">
				<label>闹钟2:</label>
			</div>
			<div class="formbox">
				<label> </label>
				<input class="form_input required" type="text" id="a2h" name="a2h" size="2" maxlength="2" value="<?php echo $row['a2h'] ?>"/><label1>:</label1>
				<input class="form_input required" type="text" id="a2m" name="a2m" size="2" maxlength="2" value="<?php echo $row['a2m'] ?>"/><label1>:</label1>
				<input class="form_input required" type="text" id="a2s" name="a2s" size="2" maxlength="2" value="<?php echo $row['a2s'] ?>"/>
			</div>
			
			<div class="formbox">
				<label>次 数: </label>
			</div>
			
			<div class="formbox">
				<label> </label>
				<input class="form_input required" type="text" id="count" name="count" size="2" maxlength="2" value="<?php echo $row['count'] ?>"/>
			</div>
			
			<div class="formbox">
				<?php if($row['state']==1) { ?>
				<p id="configstate">配置已生效，生效时间:<?php echo $row['acttime'] ?></p>
				<?php }else{ ?>
				<p id="configstate">配置未生效，配置将在十分钟后生效，配置时间:<?php echo $row['acttime'] ?></p>
				<?php } ?>
			</div>
	
            <input type="submit" name="submit" class="form_submit" id="submit" value="提交" />
            </form>
            </div>
    </div>
       
      </div>
      
    </div>
	
	

		
  </div>
  
  
</div>


