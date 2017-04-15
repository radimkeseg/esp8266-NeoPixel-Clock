/**The MIT License (MIT)
Copyright (c) 2017 by Radim Keseg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<title>NeoPixel Cuckoo</title>

<script>
 var simplePicker = {
   colorize: function(color, destID){
     if(color.match(/^#[0-9a-f]{3,6}$/)){
     document.getElementById(destID).style.backgroundColor = color;
     document.getElementById(destID).className = "color-box";
   }else{
     document.getElementById(destID).style.backgroundColor = "";
     document.getElementById(destID).className = "color-box color-box-error";
   }
   },
   
   init: function(){
    this.colorize('{val-hand-hour}','box-hand-hour');
    this.colorize('{val-hand-mins}','box-hand-mins');
    this.colorize('{val-hand-secs}','box-hand-secs');
    this.colorize('{val-segm-hour}','box-segm-hour');
   }   
  };
  
  window.onload = function(){
    simplePicker.init();
  }
</script>
 
<style>
.color-box{
    width: 32px;
    height: 20px;
    display: inline-block;
    border: 1px solid #000;
    margin-bottom: -7px;
}
.color-box-error{
    border: 1px solid #f00;
}
</style>

<body>
<h1>NeoPixel Cuckoo</h1>
<p>
Neopixel Cuckoo with an ESP8266 IoT device
<p/>

<form method='post' action='offset'>
<label>UTC TimeOffset<br><input id='_timeoffset' name='_timeoffset' length=5 pattern='^[0-9-\\.]{1,5}$' required value='{timeoffset}'></label><br>
<label>DST<br><input id='_dst' name='_dst' type='checkbox' {dst}></label><br>
<br/>
<label>Brightness<br><input id='_brightness' name='_brightness' length=3 pattern='^[0-9]{3,0}$' value='{brightness}'></label><br>
<br/>
<lable>Hand colors:</label><br/>
<label><input id="input-hand-hour" type="text" pattern="#[0-9a-f]{3,6}" name="_input-hand-hour" title="e.g. #00f or #0000ff" value="{val-hand-hour}" onkeyup="simplePicker.colorize(this.value,'box-hand-hour')" ><div class="color-box" id="box-hand-hour"></div> hours</label><br/>
<label><input id="input-hand-mins" type="text" pattern="#[0-9a-f]{3,6}" name="_input-hand-mins" title="e.g. #0f0 or #00ff00" value="{val-hand-mins}" onkeyup="simplePicker.colorize(this.value,'box-hand-mins')"><div class="color-box" id="box-hand-mins"></div> minutes</label><br/>
<label><input id="input-hand-secs" type="text" pattern="#[0-9a-f]{3,6}" name="_input-hand-secs" title="e.g. #f00 or #ff0000" value="{val-hand-secs}" onkeyup="simplePicker.colorize(this.value,'box-hand-secs')"><div class="color-box" id="box-hand-secs"></div> seconds</label><br/>
<label><input id="input-segm-hour" type="text" pattern="#[0-9a-f]{3,6}" name="_input-segm-hour" title="e.g. #f00 or #ff0000" value="{val-segm-hour}" onkeyup="simplePicker.colorize(this.value,'box-segm-hour')"><div class="color-box" id="box-segm-hour"></div> hour segment</label><br/>
<br/>
<label>ALARM<br><input id='_alarm' name='_alarm' type='checkbox' {alarm}></label><br>
<label>Time<br><input id='_alarmHour' name='_alarmHour' length=2 pattern='^[0-9]{2,0}$' value='{alarmHour}'>:<input id='_alarmMins' name='_alarmMins' length=2 pattern='^[0-9]{2,0}$' value='{alarmMins}'></label>
<br/>
<input type='submit' value='Store and Restart'></form>


</body>
</html>
)=====";
