<?php

$ourid=$_GET["id"];
$goal=$_GET["goal"];

libxml_use_internal_errors(true);
/* Createa a new DomDocument object */
$dom = new DomDocument;
/* Load the HTML */
$options = array(
  'http'=>array(
    'method'=>"GET",
    'header'=>"Accept-language: en\r\n" .
              "Cookie: foo=bar\r\n" .  // check function.stream-context-create on php.net
              "User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.102011-10-16 20:23:10\r\n" // i.e. An iPad 
  )
);
$context = stream_context_create($options);
$dat=file_get_contents("http://www.wowhead.com/quest=".$ourid, false, $context);

$dom->loadHTML($dat);
//$dom->loadHTMLFile("http://www.wowhead.com/quest=33211");
///* Create a new XPath object */
$xpath = new DomXPath($dom);
if($goal=="name" || $goal=="xp" || $goal=="reqlevel" || $goal=="type"){
	/* Query all <td> nodes containing specified class name */
	$nodes = $xpath->query("//*[@id=\"main-contents\"]/script[3]");
	
	$data=explode("\n", $nodes->item(0)->nodeValue);
	$d2=explode("{",$data[5]);
	$d3=explode("}",$d2[1]);
	$dict=json_decode("{".$d3[0]."}",TRUE);
	if($goal=="type"){
		$goal="daily";
		if(isset($dict[$goal])){
			echo "daily";
		}else{
			echo "";
		}
	}else{
		echo $dict[$goal];
	}
}else if($goal=="share"){
	$nodes = $xpath->query("//*[@id=\"main-contents\"]/div[2]/table");
	$data=$nodes->item(0)->nodeValue;
	$d1=explode("Markup.printHtml('", $data);
	$d2=explode(", 'infobox-content",$d1[1]);
	$da=explode("Not\\x20sharable",$d2[0]);
	if(count($da)==1){
		echo "Shareable";
	}else{
		echo "Not Shareable";
	}
}


?>