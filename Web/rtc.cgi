t <html><head><title>RTC Control</title></head>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("rtc.cgx", 1000);
t function plotRTCGraph() {
t  hora = document.getElementById("RTC_HORA").value;
t  fecha = document.getElementById("RTC_FECHA").value;
t }
t function periodicUpdateRTC() {
t  if(document.getElementById("RTCChkBox").checked == true) {
t   updateMultiple(formUpdate,plotRTCGraph);
t   rtc_elTime = setTimeout(periodicUpdateRTC, formUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>RTC Module Control</h2>
t <p><font size="2">En esta pagina vamos a poder realizar el cambio del RTC desde la pagina web</p>
t <form action=rtc.cgi method=post name=cgi>
t <input type=hidden value="rtc" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Hora</td>
c h 1 <td><input type=text name=rtc1 id="RTC_HORA" size=20 maxlength=20 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Fecha</TD>
c h 2 <td><input type=text name=rtc2 id="RTC_FECHA" size=20 maxlength=20 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=submit name=set value="Send" id="sbm">
t Periodic:<input type="checkbox" id="RTCChkBox" onclick="periodicUpdateRTC()">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
