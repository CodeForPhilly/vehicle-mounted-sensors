<?php

class DataPointsRequestHandler extends RecordsRequestHandler
{
    public static $recordClass = DataPoint::class;
    public static $browseOrder = ['ID' => 'DESC'];
}