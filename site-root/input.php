<?php

$data = JSON::getRequestData();


// write full data to log
file_put_contents(Site::$rootPath.'/site-data/input.log', json_encode($data).PHP_EOL, FILE_APPEND);
\Debug::dumpVar($data, false, 'wrote data to log');


// record data point to database
$DataPoint = DataPoint::create([
    'DeviceAddress' => $data['DevEUI_uplink']['DevAddr'],
    'Latitude' => $data['DevEUI_uplink']['LrrLAT'],
    'Longitude' => $data['DevEUI_uplink']['LrrLON'],
    'Payload' => $data['DevEUI_uplink']['payload_hex']
]);

$DataPoint->save();