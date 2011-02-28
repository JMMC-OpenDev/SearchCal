<?php

/*******************************************************************************
 * JMMC project
 *
 * SearchCal Proxy Service 
 * -----------------------
 *
 * "@(#) $Id: sclws-proxy.php,v 1.2 2011-02-28 12:42:31 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2010/11/04 08:49:34  mella
 * First revision
 * 
 *  
 *  By default the searchcal soap service responds on non standart http port.
 *  This script aims to be a proxy for searchcal webservice. 
 *  The service can be queried to the common http webserver which forward it to 
 *  the real soap server instance.
 * 
 *  It must be placed into the webserver directory so that it match the caller 
 *  service address (sclgui).
 *  Example: if SearchCal client calls the service using 
 *  http://apps.jmmc.fr/slcws/ , one sclws directory must serve this script.
 *  Then you should probably have following file:
 *    /var/www/html/sclws/index.php
 * 
 *  Strongly inspired from http://discuss.joyent.com/viewtopic.php?pid=184925
 */

// URL of the SOAP server
$url = $this_header['SOAPServer'];
$url = "http://jmmc.fr:8078";

// soap error message returned if searchCal server does not seems to run 
$soapErrorMsg = <<<EOM
<?xml version='1.0' encoding='UTF-8'?>
<SOAP-ENV:Envelope
xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:xsi="http://www.w3.org/1999/XMLSchema-instance"
xmlns:xsd="http://www.w3.org/1999/XMLSchema">
<SOAP-ENV:Body>
<SOAP-ENV:Fault>
<faultcode xsi:type="xsd:string">SOAP-ENV:Client</faultcode>
<faultstring xsi:type="xsd:string">
The main SearchCal server is probably down now. 
Please check again in a couple of minutes. 
If the problem still occurs, please send one feedback report (Help menu).
</faultstring>
</SOAP-ENV:Fault>
</SOAP-ENV:Body>
</SOAP-ENV:Envelope>
EOM;



//Start the Curl session
$session = curl_init($url);
//Capture all data posted
$postdata = $HTTP_RAW_POST_DATA;

// Don't return HTTP headers. Do return the contents of the call
curl_setopt($session, CURLOPT_HEADER, false);
    curl_setopt($session, CURLOPT_HTTPHEADER, $header);

// If it's a POST, put the POST data in the body
if ($postdata) {
curl_setopt ($session, CURLOPT_POSTFIELDS, $postdata);
}
// The web service returns XML. Set the Content-Type appropriately
header("Content-Type:text/xml");

// Make the call
$response = curl_exec($session);
if($response != FALSE){
  $header_size = curl_getinfo($session,CURLINFO_HEADER_SIZE);
  $result = substr($response, $header_size );
  echo $result;
}else{
  echo $soapErrorMsg;
}

curl_close($session);
?>

