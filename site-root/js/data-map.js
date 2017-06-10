(function() {
    if (!sensorData) {
        console.error('sensorData is not defined');
        return;
    }


    // global state
    var sensorDataLength = sensorData.length,
        timestamps = [], timestampMin, timestampMax, timestampDelta, timestampsLength,
        map, slider;


    // initialize application
    _analyzeData();
    _setupMap();
    _setupSlider();


    // function library
    function _analyzeData() {
        var i = 0, datum;
    
        for (; i < sensorDataLength; i++) {
            datum = sensorData[i];
            timestamps.push(datum.Created);
        }

        timestamps = timestamps.sort();
        timestampsLength = timestamps.length;

        timestampMin = timestamps[0];
        timestampMax = timestamps[timestampsLength - 1];
        timestampDelta = timestampMax - timestampMin;

        console.info('analyzed data, min=%o, max=%o, delta=%o, count=%o', timestampMin, timestampMax, timestampDelta, timestampsLength);
    }


    function _setupMap() {
        // setup map
        mapboxgl.accessToken = 'pk.eyJ1IjoidGhlbWlnaHR5Y2hyaXMiLCJhIjoiY2ozcmk4dnJ3MDAzbDJ3cjd1dGpkNXUxZSJ9.hb2cLAYkXJqdFrdzoydIpA';

        map = new mapboxgl.Map({
            container: 'map',
            style: 'mapbox://styles/mapbox/streets-v9',
            center: [-75.1694, 39.9294],
            bearing: 9.2, // Rotate Philly ~9° off of north, thanks Billy Penn.
            zoom: 12,
            maxZoom: 19,
            minZoom: 12,
            pitch: 60,
            attributionControl: true,
            touchRotate: false
        });

        console.info('map initialized:', map);
    }


    function _setupSlider() {
        var sliderRange = {
                min: timestampMin,
                max: timestampMax
            },
            i = 1, timestamp;

        for (; i < timestampsLength - 1; i++) {
            timestamp = timestamps[i];
            sliderRange[(timestamp - timestampMin) / timestampDelta * 100 + '%'] = timestamp;
        }

        slider = document.getElementById('slider');

        noUiSlider.create(slider, {
            start: timestampMax,
            tooltips: {
                to: function(value) {
                    return moment.unix(value).calendar();
                }
            },
            snap: true,
            range: sliderRange,
            pips: {
                mode: 'range',
                density: 1000,
                filter: function(value) {
                    if (value == timestampMin || value == timestampMax) {
                        return 1;
                    }
    
                    return 0;
                },
                format: {
                    to: function(value) {
                        return moment.unix(value).format('lll');
                    }
                }
            }
        });

        console.info('slider initialized:', slider);

        slider.noUiSlider.on('update', function(values) {
            console.log('slider update', values);
        });
    }
})();