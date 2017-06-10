(function() {
    if (!sensorData) {
        console.error('sensorData is not defined');
        return;
    }
    
    mapboxgl.accessToken = 'pk.eyJ1IjoidGhlbWlnaHR5Y2hyaXMiLCJhIjoiY2ozcmk4dnJ3MDAzbDJ3cjd1dGpkNXUxZSJ9.hb2cLAYkXJqdFrdzoydIpA';
    
    var map = new mapboxgl.Map({
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
    

    var min = sensorData.timestampsMin,
        max = sensorData.timestampsMax,
        diff = max - min,
        timestamps = sensorData.timestamps,
        timestampsLength = timestamps.length,
        i = 1,
        range = {
            min: min,
            max: max
        };

    for (; i < timestampsLength - 1; i++) {
        range[(timestamps[i] - min) / diff * 100 + '%'] = timestamps[i];
    }



    var slider = document.getElementById('slider');
    
    noUiSlider.create(slider, {
        start: min,
        tooltips: {
            to: function(value) {
                return moment.unix(value).calendar();
            }
        },
        snap: true,
        range: range,
        pips: {
            mode: 'range',
            density: 1000,
            filter: function(value) {
                if (value == min || value == max) {
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
})();