// Initialize your app
var myApp = new Framework7({
    animateNavBackIcon: true,
    // Enable templates auto precompilation
    precompileTemplates: true,
    // Enabled pages rendering using Template7
    template7Pages: true,
	popupCloseByOutside:true,
	cache:false,
	cacheIgnore:['setting.php']
});

// Export selectors engine
var $$ = Dom7;

// Add main View
var mainView = myApp.addView('.view-main', {
    // Enable dynamic Navbar
    dynamicNavbar: true,
});
$$(document).on('pageInit', function (e) {
  		$(".swipebox").swipebox();
		$(".videocontainer").fitVids();
		
		$("#ContactForm").validate({
		submitHandler: function(form) {
		ajaxContact(form);
		return false;
		}
		});
		

		$(".posts li").hide();	
		size_li = $(".posts li").size();
		x=3;
		$('.posts li:lt('+x+')').show();
		$('#loadMore').click(function () {
			x= (x+1 <= size_li) ? x+1 : size_li;
			$('.posts li:lt('+x+')').show();
			if(x == size_li){
				$('#loadMore').hide();
				$('#showLess').show();
			}
		});

	$("#SettingForm").submit(function(e){
		var thisform=$("#SettingForm").get(0);
		var val=thisform.a1h.value;
		if (val=="" || isNaN(val) || val<0 || val>23)
		{
			alert('请输入正确的数字');
			thisform.a1h.focus();
			return false;
		}
		val=thisform.a1m.value;
		if (val=="" || isNaN(val) || val<0 || val>59)
		{
			alert('请输入正确的数字');
			thisform.a1m.focus();
			return false;
		}
		val=thisform.a1s.value;
		if (val=="" || isNaN(val) || val<0 || val>59)
		{
			alert('请输入正确的数字');
			thisform.a1s.focus();
			return false;
		}
		val=thisform.a2h.value;
		if (val=="" || isNaN(val) || val<0 || val>23)
		{
			alert('请输入正确的数字');
			thisform.a2h.focus();
			return false;
		}
		val=thisform.a2m.value;
		if (val=="" || isNaN(val) || val<0 || val>59)
		{
			alert('请输入正确的数字');
			thisform.a2m.focus();
			return false;
		}
		val=thisform.a2s.value;
		if (val=="" || isNaN(val) || val<0 || val>59)
		{
			alert('请输入正确的数字');
			thisform.a2s.focus();
			return false;
		}
		val=thisform.count.value;
		if (val=="" || isNaN(val) || val<0 || val>30)
		{
			alert('请输入正确的数字');
			thisform.count.focus();
			return false;
		}
		myApp.popup('.popup-set');
	  return true;
	});

	function refresh_confstate() {
		var data={};
		$.ajax({
			url:'getconf_action.php',
			type:'GET', //GET
			async:true,    //或false,是否异步
			data,
			timeout:5000,    //超时时间
			dataType:'json',    //返回的数据格式：json/xml/html/script/jsonp/text
			beforeSend:function(xhr){
				console.log(xhr)
				console.log('发送前')
			},
			success:function(data,textStatus,jqXHR){
				//console.log(data)
				//console.log(textStatus)
				//console.log(jqXHR)
				var logtext="";
				if(data.state==0) {
					logtext="配置未生效，配置将在十分钟后生效，配置时间:"+data.acttime;
				} else {
					logtext = "配置已生效，生效时间:"+data.acttime;
				}
				$("#configstate").html(logtext);
				$("#a1h").val(data.a1h);
				$("#a1m").val(data.a1m);
				$("#a1s").val(data.a1s);
				$("#a2h").val(data.a2h);
				$("#a2m").val(data.a2m);
				$("#a2s").val(data.a2s);
				$("#count").val(data.count);
			},
			error:function(xhr,textStatus){
				console.log('错误')
				console.log(xhr)
				console.log(textStatus)
				alert("数据刷新失败");
			},
			complete:function(){
				myApp.pullToRefreshDone();
			}
		});
	}
	$('#setdlg').on('refresh', function (e) {
		refresh_confstate();
	});
	
	
	function loadLog(loadnew) {
		var data={page:0};
		if(loadnew==false) {
			var maxid=$("#pageno").val();
			data['page']=maxid;
		}
		$.ajax({
			url:'getlog_action.php',
			type:'GET', //GET
			async:true,    //或false,是否异步
			data,
			timeout:5000,    //超时时间
			dataType:'json',    //返回的数据格式：json/xml/html/script/jsonp/text
			beforeSend:function(xhr){
				console.log(xhr)
				console.log('发送前')
			},
			success:function(data,textStatus,jqXHR){
				if(data==null) {
					return;
				}
				console.log(data)
				console.log(textStatus)
				console.log(jqXHR)
				var logtext="";
				var type="";
				var icon="";
				
				for(var i=0;i<data.length;i++) {
					logtext="";
					type=data[i].id;
					if(data[i].type==1) {
						type+="对时";
						icon="contact";
					} else if(data[i].type==2) {
						type+="配置请求";
						icon="responsive";
					} else if(data[i].type==3) {
						type+="日志上报";
						icon="toogle";
					}
					
					if(data[i].subtype==1) {
						type+="(开机)";
					} else if(data[i].subtype==2) {
						type+="(配置上报)";
					} else if(data[i].subtype==3) {
						type+="(闹钟发生)";
					} else if(data[i].subtype==4) {
						type+="(闹钟清除)";
					} else if(data[i].subtype==5) {
						type+="(心跳)";
					} else if(data[i].subtype==6) {
						type+="(闹钟详情)";
					}
					logtext+="<li>";
					logtext+='<div class="feat_small_icon"><img src="images/icons/turquoise/'+icon+'.png" alt="" title="" /></div>';
					logtext+='<div class="feat_small_details">';
					logtext+='<h4>'+type+'</h4>';
					logtext+='<p>时间:'+data[i].recordtime+'</p>';
					logtext+='<p>内容:'+data[i].info+'</p>';
					logtext+='</div></li>';
					$("#logul").append(logtext);
					
				}
				if(loadnew==false) {
					var pageno=parseInt($("#pageno").val());
					$("#pageno").val(pageno+1);
					loading=false;
				} else {
					$("#pageno").val(2);
				}
				

			},
			error:function(xhr,textStatus){
				//console.log('错误')
				//console.log(xhr)
				//console.log(textStatus)
				//alert("数据刷新失败");
			},
			complete:function(){
				myApp.pullToRefreshDone();
				loading=false;
				$('#loadmore1').hide();
			}
		});
	}
	
	$('#logdlg').on('refresh', function (e) {
		$("#logul").find("li").remove();
		loadLog(true);
	});
	
	function onLogPageInit() {
		if($("#logul").find("li").length<=0) {
			loadLog(true);
		}
	}
	
	myApp.onPageInit('features.html', onLogPageInit());
	
	
	// 加载flag
	var loading = false;
	
	 
	// 注册'infinite'事件处理函数
	$('#loadmore').on('click', function () {
	 
	  // 如果正在加载，则退出
	  if (loading) return;
	  // 设置flag
	  $('#loadmore1').show();
	  loading = true;
	  loadLog(false);
	});  

	
	$('.popup-set').on('opened', function () {
		console.log('Set Popup is opened');
		var data={};
		$("#SettingForm input").each(function(){
			data[$(this).attr('name')]=$(this).val();
		});
			   
		$.ajax({
			url:'conf_action.php',
			type:'POST', //GET
			async:true,    //或false,是否异步
			data,
			timeout:5000,    //超时时间
			dataType:'json',    //返回的数据格式：json/xml/html/script/jsonp/text
			beforeSend:function(xhr){
				console.log(xhr)
				console.log('发送前')
			},
			success:function(data,textStatus,jqXHR){
				console.log(data)
				console.log(textStatus)
				console.log(jqXHR)
				$("#res_note").html(textStatus);
			},
			error:function(xhr,textStatus){
				console.log('错误')
				console.log(xhr)
				console.log(textStatus)
				var textLog='<font color="#FF0000">'+textStatus+'</font>';
				$("#res_note").html(textLog);
			},
			complete:function(){
				var test=$("#close_setpopu");
				$("#close_setpopu").show();
			}
		})

	});
	
	$('.popup-set').on('closed', function () {
		console.log('Set Popup is closing')
		refresh_confstate();
	});
	/*
	$("form.ajax-submit").on('submitted', function (e) {
		var xhr = e.detail.xhr; // actual XHR object


		//var data = e.detail.data; // Ajax repsonse from action file
		// do something with response data
		alert("test");
	});*/
	
	$("a.switcher").bind("click", function(e){
		e.preventDefault();
		
		var theid = $(this).attr("id");
		var theproducts = $("ul#photoslist");
		var classNames = $(this).attr('class').split(' ');
		
		
		if($(this).hasClass("active")) {
			// if currently clicked button has the active class
			// then we do nothing!
			return false;
		} else {
			// otherwise we are clicking on the inactive button
			// and in the process of switching views!

  			if(theid == "view13") {
				$(this).addClass("active");
				$("#view11").removeClass("active");
				$("#view11").children("img").attr("src","images/switch_11.png");
				
				$("#view12").removeClass("active");
				$("#view12").children("img").attr("src","images/switch_12.png");
			
				var theimg = $(this).children("img");
				theimg.attr("src","images/switch_13_active.png");
			
				// remove the list class and change to grid
				theproducts.removeClass("photo_gallery_11");
				theproducts.removeClass("photo_gallery_12");
				theproducts.addClass("photo_gallery_13");

			}
			
			else if(theid == "view12") {
				$(this).addClass("active");
				$("#view11").removeClass("active");
				$("#view11").children("img").attr("src","images/switch_11.png");
				
				$("#view13").removeClass("active");
				$("#view13").children("img").attr("src","images/switch_13.png");
			
				var theimg = $(this).children("img");
				theimg.attr("src","images/switch_12_active.png");
			
				// remove the list class and change to grid
				theproducts.removeClass("photo_gallery_11");
				theproducts.removeClass("photo_gallery_13");
				theproducts.addClass("photo_gallery_12");

			} 
			else if(theid == "view11") {
				$("#view12").removeClass("active");
				$("#view12").children("img").attr("src","images/switch_12.png");
				
				$("#view13").removeClass("active");
				$("#view13").children("img").attr("src","images/switch_13.png");
			
				var theimg = $(this).children("img");
				theimg.attr("src","images/switch_11_active.png");
			
				// remove the list class and change to grid
				theproducts.removeClass("photo_gallery_12");
				theproducts.removeClass("photo_gallery_13");
				theproducts.addClass("photo_gallery_11");

			} 
			
		}

	});	
	
	document.addEventListener('touchmove', function(event) {
	   if(event.target.parentNode.className.indexOf('navbarpages') != -1 || event.target.className.indexOf('navbarpages') != -1 ) {
		event.preventDefault(); }
	}, false);
	
	
	
	
	var ScrollFix = function(elem) {
		// Variables to track inputs
		var startY = startTopScroll = deltaY = undefined,
	
		elem = elem || elem.querySelector(elem);
	
		// If there is no element, then do nothing	
		if(!elem)
			return;
	
		// Handle the start of interactions
		elem.addEventListener('touchstart', function(event){
			startY = event.touches[0].pageY;
			startTopScroll = elem.scrollTop;
	
			if(startTopScroll <= 0)
				elem.scrollTop = 1;
	
			if(startTopScroll + elem.offsetHeight >= elem.scrollHeight)
				elem.scrollTop = elem.scrollHeight - elem.offsetHeight - 1;
		}, false);
	};
	
	$('#contactme').click(function(){
		window.location.href='http://www.log4cpp.com/smarthome/52.html';
	});
	// Add ScrollFix
	var scrollingContent = document.getElementById("pages_maincontent");
	new ScrollFix(scrollingContent);
		
		
})
