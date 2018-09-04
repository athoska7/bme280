<?php
$servername = "localhost";
$username = "";
$password = "";
$dbname = "";
	
if(isset($_REQUEST['data']) && !empty($_REQUEST['data']))
{
	$Data = json_decode(base64_decode($_REQUEST['data']));
	if(!isset($Data->location))
	{
		$Data->{"location"} = 0;
	}
	//print_r(base64_decode($_REQUEST['data']));
	//print_r($Data);
	SaveSensorData($Data->temperature, $Data->humidity, $Data->pressure, $Data->location);
}
else if(isset($_REQUEST['getnewdata']))
{
	try 
	{
		$response = array();
		$conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
		// set the PDO error mode to exception
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql="SELECT * FROM (SELECT * FROM sensordata  WHERE LocationID = :locationid ORDER BY Date DESC LIMIT 1440) tmp ORDER BY tmp.ID ASC";
		$query = $conn->prepare($sql);
		$parameters = array(':locationid' => $_REQUEST['getnewdata']);
		$query->execute($parameters);
		$datas = $query->fetchAll();
		foreach ($datas as $data)
		{
			$temeperature[] = $data['Temperature'];
			$humidity[] = $data['Humidity'];
			$pressure[] = $data['Pressure'] + 13.42;
			$TimeStamp[] = $data['Date'];
		}
		$sql="SELECT Temperature, Humidity, (Pressure + 13.42) AS Pressure, Date FROM sensordata WHERE LocationID = :locationid ORDER BY Date DESC LIMIT 1";
		$query = $conn->prepare($sql);
		$parameters = array(':locationid' => $_REQUEST['getnewdata']);
		$query->execute($parameters);
		$LastMeasurement = $query->fetch();
		
		$response = array('Temperature' => $temeperature,
			'Humidity' => $humidity,
			'Pressure' => $pressure,
			'Date' => $TimeStamp,
			'LastMeasurement' => $LastMeasurement);
		$arr = array('succes' => 'true', 'ErrorMessage' => "", 'response' => $response);										
		echo json_encode($arr);
	}
	catch(PDOException $e)
	{
		echo "Error: " . $e->getMessage();
		$arr = array('succes' => "false", 'ErrorMessage' => $e->getMessage(), 'response' => "");										
		echo json_encode($arr);
	}
	$conn = null;
}
else if(isset($_REQUEST['getlocations']))
{
	try{
		$conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
		// set the PDO error mode to exception
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		// prepare sql and bind parameters
		$sql="SELECT * FROM Locations";
		$query = $conn->prepare($sql);
		$query->execute();
		$Locations = $query->fetchAll(PDO::FETCH_OBJ);
		$arr = array('succes' => 'true', 'ErrorMessage' => "", 'response' => $Locations);										
		echo json_encode($arr);
	}
	catch(PDOException $e)
	{
		echo "Error: " . $e->getMessage();
		$arr = array('succes' => "false", 'ErrorMessage' => $e->getMessage(), 'response' => "");										
		echo json_encode($arr);
	}
	$conn = null;
}


function SaveSensorData($Temperature, $Humidity, $Pressure, $Location)
{
	try {
		$conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
		// set the PDO error mode to exception
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		// prepare sql and bind parameters
		$sql="INSERT INTO sensordata(Temperature, Humidity, Pressure, LocationID) VALUES(:temp, :hum, :press, :loc)";
		$query = $conn->prepare($sql);
	 	$parameters = array(':temp' => $Temperature, ':hum' =>$Humidity, 'press' => $Pressure, 'loc' => $Location);	
	 	$query->execute($parameters);
		}
	catch(PDOException $e)
		{
		echo "Error: " . $e->getMessage();
		}
	$conn = null;
}
?>
