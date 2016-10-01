<?php

require("inc/phpMQTT.php");

function send_error($msg) {
	http_response_code(500);
	header("Content-Type: application/json");
	die('{ "error": "' . $msg . '" }');
}

function send_result($data) {
	header("Content-Type: application/json");
	echo json_encode(array("error" => "", "data" => $data));
}

function check_parameter($name, $allowed_values) {
	if(isset($_GET[$name]) == false) {
		send_error("Parameter '" . $name . "' is missing");
	}

	$value = $_GET[$name];
	if(in_array($value, $allowed_values) == false) {
		send_error("Invalid value '" . $value . "' for parameter '" . $name . "'"); 
	}

	return $value;
}

function connect_mqtt() {
	$mqtt = new phpMQTT("10.23.42.10", 1883, "c3ma-lamp-controller-" . uniqid());
	if($mqtt->connect()) {
		return $mqtt;
	}
	else {
		send_error("Failed to connect to MQTT broker");
	}
}

$mqtt_messages = array();
function save_messages($topic, $msg) {
	global $mqtt_messages;
	$mqtt_messages[$topic] = $msg;
}


$elements = array("1", "2", "3", "4", "5", "6", "main", "workshop");


$command = check_parameter("command", array("query", "update"));

// Abfrage des Lampenstatus
if($command == "query") {
	$mqtt = connect_mqtt();

	$start = time();

	$data = array();
	foreach($elements as $key) {
		$data[$key] = "unknown";
	}

	$topics['/room/light/+/state'] = array("qos" => 0, "function" => "save_messages");
	$mqtt->subscribe($topics, 0);
	while($mqtt->proc()) {
		// Abbrechen, wenn zu viel Zeit vergangen ist
		if(time() > ($start + 3)) {
			break;
		} 

		// MQTT in Ergebnis kopieren
		foreach($mqtt_messages as $topic => $msg) {
			$key = explode("/", $topic)[3];
			if(isset($data[$key]) == false) continue; 
			$data[$key] = $msg;
		}	

		// Prüfen, ob die Daten vollständig sind
		$completed = true;
		foreach($data as $value) {
			if($value == "unknown") {
				$completed = false;
				break;
			}
		}
		if($completed == true) {
			break;
		}
	}
	$mqtt->close();
	send_result($data);
}

// Schalten einer Lampe / Lampengruppe
if($command == "update") {
	$element = check_parameter("element", $elements);
	$value = check_parameter("value", array("on", "off"));

	$mqtt = connect_mqtt();
	$mqtt->publish("/room/light/" . $element . "/command", $value, 0);
	$mqtt->close();

	send_result(true);
}

