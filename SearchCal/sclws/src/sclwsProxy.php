<?php
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/** 
 * If the $filename exists, then each soap client requests are appended 
 * (you would probably have to set permission to allow apache writing )
 */
function logInFile($data){
    $filename = 'sclwsProxy.log';
    if (is_writable($filename)) {
        if ($handle = fopen($filename, 'a')) {
            fwrite($handle, "<e>\n<date>".date("c")."</date>\n<ip>".$_SERVER['REMOTE_ADDR']."</ip>\n".preg_replace('~.*?\?>~', '', $data)."\n</e>\n");
        }
    }
}

/**
 * By default the searchcal SOAP service responds on non standard HTTP port.
 *
 * This script aims to be a proxy for SearchCal webservice. 
 * The service can be queried on the common HTTP webserver, which forwards it to 
 * the real SOAP server instance.
 * 
 * It must be placed into the webserver directory so that it matches the caller 
 * service address (see sclgui).
 *
 * Example: if SearchCal client calls the service using 
 * http://apps.jmmc.fr/slcws/ , one sclws directory must serve this script.
 * Then you should probably have the following file:
 *   /var/www/html/sclws/sclws-proxy.php
 * 
 * Strongly inspired from http://discuss.joyent.com/viewtopic.php?pid=184925
 */

// URL of the SOAP server
$url = $this_header['SOAPServer'];
$url = "http://jmmc.fr:8078";

// SOAP error message returned if SearchCal server does not seem to run 
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

// Start the CURL session
$session = curl_init($url);

// Capture all data posted
$postdata = $HTTP_RAW_POST_DATA;

// Don't return HTTP headers. Do return the contents of the call
curl_setopt($session, CURLOPT_HEADER, false);

// Set one timeout for housekeeping 
curl_setopt($session, CURLOPT_TIMEOUT, 7200);

// If it's a POST, put the POST data in the body
if ($postdata) {
    curl_setopt ($session, CURLOPT_POSTFIELDS, $postdata);
}

logInFile($postdata);

// The web service returns XML. Set the Content-Type appropriately
header("Content-Type:text/xml");

// Make the call
$response = curl_exec($session);

if ($response != FALSE) {
    $header_size = curl_getinfo($session,CURLINFO_HEADER_SIZE);
    $result = substr($response, $header_size );
    echo $result;
} else {
    echo $soapErrorMsg;
}

// Close the CURL session
curl_close($session);
?>
