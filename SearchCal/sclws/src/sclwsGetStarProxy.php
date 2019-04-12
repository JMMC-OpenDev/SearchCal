<?php
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

function xmldecode($txt)
{
    $txt = str_replace('&#xA;', "\n", $txt);
    $txt = str_replace('&lt;',  '<',  $txt);
    $txt = str_replace('&gt;',  '>',  $txt);
    $txt = str_replace('&amp;', '&',  $txt); /* after &lt; and &gt; to avoid double-decoding */
    return $txt;
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
// dev:
// $url = "http://127.0.0.1:6666";
// beta:
// $url = "http://127.0.0.1:7079";
// prod:
$url = "http://127.0.0.1:8079";


// Convert Get query to a SOAP server status request:
$star = $_GET['star'];

// Parse output format:
$format = $_GET['format'];
if (empty($format)
    || (strcmp($format, "tsv") != 0 && strcmp($format, "vot") != 0) {
    $format = "vot";
}

if (empty($star)
    || strcmp($star, "INTERNAL") == 0
    || strcmp($star, "UNKNOWN") == 0
    || strcmp($star, "No_name") == 0)
{
    // invalid parameter
    header("HTTP/1.0 500 Internal Server Error");
    header("Content-Type:text/html");

    echo <<<EOM
<?xml version='1.0' encoding='UTF-8'?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<body>
The given star parameter is invalid:
<br/>'
EOM;
    echo $star;
    echo <<<EOM
'</body>
</html>
EOM;

} else {

    // SOAP error message returned if SearchCal server does not seem to run
    $soapErrorMsg = <<<EOM
<?xml version='1.0' encoding='UTF-8'?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<body>
The SearchCal Server is probably down now.
Please check again in a couple of minutes.
If the problem still occurs, please send a feedback report.
</body>
</html>
EOM;

    // Start the CURL session
    $session = curl_init($url);

    // Don't return HTTP headers. Do return the contents of the call
    curl_setopt($session, CURLOPT_HEADER, false);

    // Set one timeout for housekeeping (5 minutes)
    curl_setopt($session, CURLOPT_TIMEOUT, 300);

    //Set curl to return the data instead of printing it to the browser.
    curl_setopt($session, CURLOPT_RETURNTRANSFER, 1);

    $soapGetStarMsg = <<<EOM
<?xml version="1.0" encoding="UTF-8"?>
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <soapenv:Body>
    <GetStar xmlns="urn:sclws">
     <query>-objectName $star -format $format</query>
    </GetStar>
  </soapenv:Body>
</soapenv:Envelope>
EOM;

    curl_setopt ($session, CURLOPT_POSTFIELDS, $soapGetStarMsg);


    // Make the call
    $soapResponse = curl_exec($session);

    if ($soapResponse != FALSE) {
        $tagStart = '<output>';
        $tagEnd   = '</output>';

        $soapOutputFrom = strpos($soapResponse, $tagStart);
        $soapOutputEnd  = strpos($soapResponse, $tagEnd);

        if (($soapOutputFrom === false) || ($soapOutputEnd === false)) {
            $tagStart = '<faultstring>';
            $tagEnd   = '</faultstring>';
            $soapFaultFrom = strpos($soapResponse, $tagStart);
            $soapFaultEnd  = strpos($soapResponse, $tagEnd);

            header("HTTP/1.0 500 Internal Server Error");
            header("Content-Type:text/html");

            if (($soapFaultFrom === false) || ($soapFaultEnd === false)) {
                echo $soapErrorMsg;
            } else {
                $soapFaultFrom += strlen($tagStart);
                $soapFault = substr($soapResponse, $soapFaultFrom, $soapFaultEnd - $soapFaultFrom);
                echo <<<EOM
<?xml version='1.0' encoding='UTF-8'?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<body>
The SearchCal Server returned the error:
<hr/>
EOM;
                echo $soapFault;
                echo <<<EOM
<hr/>
If the problem still occurs, please send a feedback report.
</body>
</html>
EOM;
            }
        } else {
            $soapOutputFrom += strlen($tagStart);

            /* decode xml encoded elements from soap body */
            $output = xmldecode(substr($soapResponse, $soapOutputFrom, $soapOutputEnd - $soapOutputFrom));

            $xmlVotableFrom = strpos($output, '<VOTABLE');

            if ($xmlVotableFrom === false) {
                // The web service returns TSV. Set the Content-Type appropriately
                header("Content-Type:text/plain");
                echo $output;
            } else {
                // The web service returns XML. Set the Content-Type appropriately
                header("Content-Type:text/xml");

                $content = substr($output, $xmlVotableFrom);
                echo '<?xml version="1.0" encoding="UTF-8"?>';
                echo '<?xml-stylesheet href="./getstarVOTableToHTML.xsl" type="text/xsl"?>';
                echo $content;
            }
        }
    } else {
        header("HTTP/1.0 500 Internal Server Error");
        header("Content-Type:text/html");
        echo $soapErrorMsg;
    }

    // Close the CURL session
    curl_close($session);
}
?>
