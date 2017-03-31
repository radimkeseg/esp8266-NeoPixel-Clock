const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<title>NeoPixel Cuckoo</title>
<body>
<h1>NeoPixel Cuckoo</h1>
<p>
Neopixel Cuckoo with an ESP8266 IoT device
<p/>

<form method='post' action='offset'>
<label>UTC TimeOffset<br><input id='_timeoffset' name='_timeoffset' length=5 pattern='^[0-9-\\.]{1,5}$' required value='{timeoffset}'></label><br>
<label>DST<br><input id='_dst' name='_dst' type='checkbox' {dst}></label><br>
<input type='submit' value='Store and Restart'></form>


</body>
</html>
)=====";
