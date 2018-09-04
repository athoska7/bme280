<body>
	<meta name="viewport" content="width=device-width" />
	<head>
	<style>
		.stats{
			width:80%; margin:auto;
		}
		@media screen and (max-width: 500px) {
			.stats {
				width:95%; margin:auto;
			}
		}
	</style>
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js"></script>
		<script src="chart.min.js"></script>
		<script src="jquery-cookie.js"></script>
		
		
		<script>
		var datatemp;
		var datahum;
		var datapress;
		var TempChart;
		var HumChart;
		var PressChart;
		var LastRefresh;
		
		var options = {
			scales: {
				yAxes: [{
					ticks: {
						//beginAtZero: true
					}
				}]
			}
		  }
		  
		function DrawGraphs(){
			$.ajax({
				type: 'get',
				dataType: "JSON",
				url: 'http://idojaras.gradualisportal.com/sensor.php',
				data: {getnewdata: $( "#Location" ).val()},
				cache: false,
				success: function(response){
					datatemp = {
					labels : response.response.Date,
						datasets : [
						  {
								label : "Hőmérséklet (°C)",
								fill:false,
								borderColor : "#0073aa",
								pointBorderColor : "#0073aa",
								pointBackgroundColor : "#0073aa",
								pointRadius :0,
								backgroundColor : "#fff",
								data :response.response.Temperature,
							},
						]
					}
				  
					datahum = {
							labels : response.response.Date,
							datasets : [
							{
								label : "Páratartalom (%)",
								fill:false,
								borderColor : "#0073aa",
								pointBorderColor : "#0073aa",
								pointBackgroundColor : "#0073aa",
								pointRadius :0,
								backgroundColor : "#fff",
								data :response.response.Humidity,
							},
						]
					}
				  
					datapress = {
							labels : response.response.Date,
							datasets : [
							{
								label : "Légnyomás (kPa)",
								fill:false,
								borderColor : "#0073aa",
								pointBorderColor : "#0073aa",
								pointBackgroundColor : "#0073aa",
								pointRadius :0,
								backgroundColor : "#fff",
								data :response.response.Pressure,
							},
						]
					}
				  
				  //Get the context of the canvas element we want to select
				  var temeperature = document.getElementById("temeperature").getContext("2d");
				  TempChart = Chart.Line(temeperature, {data: datatemp, options: options});
				  
				  
				  var humidity = document.getElementById("humidity").getContext("2d");
				  HumChart = Chart.Line(humidity, {data: datahum, options: {
					  scales: {
						yAxes: [{
							ticks: {
								min: 0,
								max: 100
							}
						}]
						}
					}
				  
				  });
				  
					var pressure = document.getElementById("pressure").getContext("2d");
					PressChart = Chart.Line(pressure, {data: datapress});
					
					LastRefresh = response.response.LastMeasurement.Date;
					$("#lastm b").text(response.response.LastMeasurement.Date);
					$("#temp b").text(Number(response.response.LastMeasurement.Temperature).toFixed(2) + " °C");
					$("#hum b").text(Number(response.response.LastMeasurement.Humidity).toFixed(2) + " %");
					$("#press b").text(Number(response.response.LastMeasurement.Pressure).toFixed(2) + " hPa");
					TempChart.update();
					HumChart.update();
					PressChart.update();
				}
			});
		}	
		
		jQuery(document).ready(function($){
			var Location = $.cookie("Location");
			if(Location != 0){			
				jQuery("#Location").prop('selectedIndex', Location);
			}
			DrawGraphs();
			
			jQuery("#Location").on("change", function(){
				$.cookie("Location", this.value);
				DrawGraphs();
			});
			
		  setInterval(function(){
			  $.ajax({
				type: 'get',
				dataType: "JSON",
				url: 'http://idojaras.gradualisportal.com/sensor.php',
				data: {getnewdata: $( "#Location" ).val()},
				cache: false,
				success: function(response){
					
					if(new Date(LastRefresh).getTime() < new Date(response.response.LastMeasurement.Date).getTime())	//Refresh if new data avaible
					{
						datatemp.labels.shift();
						datatemp.labels.push(response.response.LastMeasurement.Date);
						datatemp.datasets[0].data.shift();
						datatemp.datasets[0].data.push(response.response.LastMeasurement.Temperature);
						
						datahum.labels.shift();
						datahum.labels.push(response.response.LastMeasurement.Date);
						datahum.datasets[0].data.shift();
						datahum.datasets[0].data.push(response.response.LastMeasurement.Humidity);
						
						datapress.labels.shift();
						datapress.labels.push(response.response.LastMeasurement.Date);
						datapress.datasets[0].data.shift();
						datapress.datasets[0].data.push(response.response.LastMeasurement.Pressure);

						
						$("#lastm b").text(response.response.LastMeasurement.Date);
						$("#temp b").text(Number(response.response.LastMeasurement.Temperature).toFixed(2) + " °C");
						$("#hum b").text(Number(response.response.LastMeasurement.Humidity).toFixed(2) + " %");
						$("#press b").text(Number(response.response.LastMeasurement.Pressure).toFixed(2) + " hPa");
						TempChart.update();
						HumChart.update();
						PressChart.update();
					}
				}
				});
			  
			}, 60000);
		  
		  
		});
		
		
		

		</script>
	</head>
	<div class="stats">
		<h1>Időjárás</h1>
		<div style="width: 300px; margin: 0 auto;">
				<p>Mérőállomás:
					<select name="Location" id="Location">
						<option value="0">Viktor</option>
						<option value="1">Gyuri</option>
						<option value="2">Gyuri2</option>
					</select>
				</p>
				<p id="lastm">Utolsó mérés: <b></b></p>
				<p id="temp">Hőmérséklet: <b> °C</b></p>
				<p id="hum">Páratartalom: <b> %</b></p>
				<p id="press">Légnyomás: <b> hPa</b></p>
		</div>
		<canvas id="temeperature" height="500" width="900"></canvas>			
		<canvas id="humidity" height="500" width="900"></canvas>
		<canvas id="pressure" height="500" width="900"></canvas>	
	</div>
</body>




