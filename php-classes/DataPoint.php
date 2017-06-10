<?php

class DataPoint extends ActiveRecord
{
    public static $tableName = 'datapoints';
    public static $singularNoun = 'data point';
    public static $pluralNoun = 'data points';

    public static $fields = [
        'DeviceAddress' => [
            'type' => 'string',
            'index' => true
        ],
        'Latitude' => [
            'type' => 'decimal',
            'length' => '10, 8'
        ],
        'Longitude' => [
            'type' => 'decimal',
            'length' => '11, 8'
        ],
        'Payload' => [
            'type' => 'string'
        ]
    ];
}