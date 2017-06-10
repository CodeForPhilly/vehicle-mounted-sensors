{extends designs/site.tpl}

{block title}Data Points &mdash; {$dwoo.parent}{/block}

{block user-tools}{/block}
{block header-bottom}{/block}

{block js-top}
    <link href='https://api.mapbox.com/mapbox-gl-js/v0.38.0/mapbox-gl.css' rel='stylesheet' />
    {cssmin "nouislider.css"}
{/block}

{block js-bottom}
    <script src='https://api.mapbox.com/mapbox-gl-js/v0.38.0/mapbox-gl.js'></script>
    <script>
        var sensorData = <?php
            $timestamps = [];

            foreach ($this->scope['data'] AS $DataPoint) {
                if (!in_array($DataPoint->Created, $timestamps)) {
                    $timestamps[] = $DataPoint->Created;
                }
            }

            sort($timestamps);

            echo json_encode([
                'timestampsMin' => min($timestamps),
                'timestampsMax' => max($timestamps),
                'timestamps' => $timestamps
            ]);
        ?>;
    </script>
    {jsmin "moment.js+wnumb.js+nouislider.js+data-map.js"}
{/block}

{block content}
    <header class="page-header">
        <h1 class="header-title title-1">Data Points</h1>
    </header>

    <div id='slider' style='margin-bottom: 50px; margin-top: 60px'></div>
    <div id='map' style='height: 300px;'></div>

    <table class="row-stripes cell-borders">
        <thead>
            <tr>
                <th>Timestamp</th>
                <th>Device Address</th>
                <th>Latitude</th>
                <th>Longitude</th>
                <th>Raw Payload</th>
                <th>Decoded</th>
            </tr>
        </thead>

        <tbody>
        {foreach item=DataPoint from=$data}
            <tr>
                <td>{$DataPoint->Created|date_format:'%Y-%m-%d %H:%M:%S'}</td>
                <td>{$DataPoint->DeviceAddress}</td>
                <td>{$DataPoint->Latitude}</td>
                <td>{$DataPoint->Longitude}</td>
                <td><code>{$DataPoint->Payload}</code></td>
                <td>
                    <dl>
                        <dt>Relative Humidity</dt>
                        <dd>{$DataPoint->Payload|substr:6:2} &rarr; {$DataPoint->Payload|substr:6:2|hexdec}</dd>
                        <dt>Temperature</dt>
                        <dd>{$DataPoint->Payload|substr:12:2} &rarr; {$DataPoint->Payload|substr:12:2|hexdec}</dd>
                    </dl>
                </td>
            </tr>
        {/foreach}
        </tbody>
    </table>
{/block}