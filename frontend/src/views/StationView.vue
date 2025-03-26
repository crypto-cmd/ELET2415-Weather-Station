<template>
  <div class="station-view">
    <h2>Live Station Data</h2>

    <div class="unit-toggle">
      <label>Units: </label>
      <button @click="units = 'metric'" :class="{ active: units === 'metric' }">Metric (°C, hPa)</button>
      <button @click="units = 'imperial'" :class="{ active: units === 'imperial' }">Imperial (°F, inHg)</button>
    </div>

    <div v-if="loading" class="loading">Loading latest data...</div>
    <div v-if="error" class="error">Error fetching data: {{ error }}</div>

    <div v-if="latestData" class="live-data-grid">
      <WeatherCard title="Temperature" :value="displayTemp" :unit="displayUnits.temp"
        :secondaryValue="displaySecondaryTemp" :secondaryUnit="displayUnits.secondaryTemp" />
      <WeatherCard title="Humidity" :value="latestData.humidity" unit="%" :precision="0" />
      <WeatherCard title="Pressure" :value="displayPressure" :unit="displayUnits.pressure"
        :secondaryValue="displaySecondaryPressure" :secondaryUnit="displayUnits.secondaryPressure" :precision="1" />
      <WeatherCard title="Altitude" :value="displayAltitude" :unit="displayUnits.altitude"
        :secondaryValue="displaySecondaryAltitude" :secondaryUnit="displayUnits.secondaryAltitude" :precision="1" />
      <WeatherCard title="Heat Index" :value="displayHeatIndex" :unit="displayUnits.temp"
        :secondaryValue="displaySecondaryHeatIndex" :secondaryUnit="displayUnits.secondaryTemp" />
      <WeatherCard title="Soil Moisture" :value="latestData.moisture" unit="%" :precision="0" />
    </div>
    <div v-if="latestData" class="live-data-grid">
      <AiSuggestionCard :weatherData="latestData" class="ai-card" />
      <div class="timestamp-card">
        <h4>Last Update</h4>
        <p>{{ formattedTimestamp }}</p>
      </div>
    </div>

    <div class="chart-container">
      <h3>Recent {{ selectedChartMetric.charAt(0).toUpperCase() + selectedChartMetric.slice(1) }} Trend</h3>

      <div class="chart-metric-selector">
        <label for="chartMetric">Show Trend For: </label>
        <select id="chartMetric" v-model="selectedChartMetric">
          <option value="temperature">Temperature</option>
          <option value="humidity">Humidity</option>
          <option value="pressure">Pressure</option>
          <option value="heatIndex">Heat Index</option>
          <option value="moisture">Moisture</option>
          <option value="altitude">Altitude</option>
        </select>
      </div>

      <LineChart v-if="chartData.labels && chartData.labels.length > 1" :chart-data="chartData"
        :chart-options="chartOptions" />
      <p v-else>Collecting data for chart...</p>
    </div>

  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, computed, reactive, watch } from 'vue';

import WeatherCard from '@/components/WeatherCard.vue';
import AiSuggestionCard from '@/components/AiSuggestionCard.vue';
import LineChart from '@/components/LineChart.vue'; // Import chart component

import { useMqttStore } from "@/store/mqttStore"; // Assuming you have a store for MQTT
import { useAppStore } from '@/store/appStore'; // Assuming you have a store for global state
import { storeToRefs } from "pinia";

const { celsiusToFahrenheit, pascalsToHpa, pascalsToInHg, metersToFeet, formatTimestamp, getLatestData } = useAppStore(); // Assuming you have a store for global state

const Mqtt = useMqttStore();
const { payload } = storeToRefs(Mqtt); // Assuming you have a store for MQTT
const latestData = ref(null);
const loading = ref(true);
const error = ref(null);
let intervalId = null;
const units = ref('metric'); // 'metric' or 'imperial'
const selectedChartMetric = ref('temperature'); // Default metric for the chart


// For the real-time chart
const MAX_CHART_POINTS = 200; // Show last 60 seconds (adjust as needed)
// Expand chartHistory to hold arrays for all plottable metrics
const chartHistory = reactive({
  timestamps: [],
  temperature: [],
  humidity: [],
  pressure: [], // Store raw Pascal values
  heatIndex: [],
  moisture: [],
  altitude: []  // Store raw meter values
});

const fetchData = async () => {
  error.value = null;
  try {
    const data = await getLatestData(); // Fetch latest data from the store
    latestData.value = data;

    // Update chart history for ALL metrics
    const now = new Date(data.timestamp * 1000);
    chartHistory.timestamps.push(now);
    chartHistory.temperature.push(data.temperature);
    chartHistory.humidity.push(data.humidity);
    chartHistory.pressure.push(data.pressure);
    chartHistory.heatIndex.push(data.heatIndex);
    chartHistory.moisture.push(data.moisture);
    chartHistory.altitude.push(data.altitude);

    // Keep history length limited for ALL arrays
    if (chartHistory.timestamps.length > MAX_CHART_POINTS) {
      chartHistory.timestamps.shift();
      chartHistory.temperature.shift();
      chartHistory.humidity.shift();
      chartHistory.pressure.shift();
      chartHistory.heatIndex.shift();
      chartHistory.moisture.shift();
      chartHistory.altitude.shift();
    }

  } catch (err) {
    error.value = err.message || 'Failed to fetch data';
  } finally {
    // Only set loading false initially, not on subsequent fetches
    if (loading.value) loading.value = false;
  }
};

watch(payload, (newPayload) => {
  if (newPayload) {
    // fetchData(newPayload); // Fetch data when payload changes
    latestData.value = newPayload; // Update latest data with new payload
    console.log('MQTT payload received:', newPayload);
  }
});

onMounted(() => {
  Mqtt.connect(); // Connect to MQTT broker
  fetchData(); // Initial fetch
  intervalId = setInterval(fetchData, 1000); // Refresh every 1 second
  console.log('StationView mounted, starting refresh interval.');

  setTimeout(() => {
    Mqtt.subscribe("620164974");
  }, 5000);
});

onUnmounted(() => {
  if (intervalId) {
    // clearInterval(intervalId);
    console.log('StationView unmounted, clearing refresh interval.');
  }
  Mqtt.unsubcribeAll(); // Unsubscribe from all topics

});

const formattedTimestamp = computed(() => {
  return latestData.value ? formatTimestamp(latestData.value.timestamp) : 'N/A';
});

// Computed properties for unit conversion and display
const displayUnits = computed(() => {
  if (units.value === 'imperial') {
    return {
      temp: '°F', secondaryTemp: '°C',
      pressure: 'inHg', secondaryPressure: 'hPa',
      altitude: 'ft', secondaryAltitude: 'm',
    };
  }
  return { // Metric
    temp: '°C', secondaryTemp: '°F',
    pressure: 'hPa', secondaryPressure: 'inHg',
    altitude: 'm', secondaryAltitude: 'ft',
  };
});

// --- Computed values for display ---
const displayTemp = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? celsiusToFahrenheit(latestData.value.temperature)
    : latestData.value.temperature;
});
const displaySecondaryTemp = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? latestData.value.temperature // Show Celsius as secondary
    : celsiusToFahrenheit(latestData.value.temperature); // Show F as secondary
});

const displayHeatIndex = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? celsiusToFahrenheit(latestData.value.heatIndex)
    : latestData.value.heatIndex;
});
const displaySecondaryHeatIndex = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? latestData.value.heatIndex
    : celsiusToFahrenheit(latestData.value.heatIndex);
});

const displayPressure = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? pascalsToInHg(latestData.value.pressure)
    : pascalsToHpa(latestData.value.pressure);
});
const displaySecondaryPressure = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? pascalsToHpa(latestData.value.pressure)
    : pascalsToInHg(latestData.value.pressure);
});

const displayAltitude = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? metersToFeet(latestData.value.altitude)
    : latestData.value.altitude;
});
const displaySecondaryAltitude = computed(() => {
  if (!latestData.value) return null;
  return units.value === 'imperial'
    ? latestData.value.altitude
    : metersToFeet(latestData.value.altitude);
});
// --- End Computed values ---

// Computed property for Chart.js data structure
const chartData = computed(() => {
  const metricKey = selectedChartMetric.value; // e.g., 'temperature', 'humidity'

  // Check if the selected metric's history exists and has data
  if (!chartHistory[metricKey] || chartHistory[metricKey].length === 0) {
    return { labels: [], datasets: [] }; // Return empty structure if no data
  }

  let dataPoints = [];
  let unitLabel = '';
  const sourceData = chartHistory[metricKey]; // Get the raw data array (e.g., chartHistory.temperature)

  // Process data points: Apply unit conversions based on selected metric and units
  dataPoints = sourceData.map(value => {
    if (value === null || value === undefined) return null; // Handle potential nulls in history

    switch (metricKey) {
      case 'temperature':
      case 'heatIndex':
        unitLabel = displayUnits.value.temp; // Use pre-calculated unit label (°C or °F)
        return units.value === 'imperial' ? celsiusToFahrenheit(value) : value;
      case 'pressure':
        unitLabel = displayUnits.value.pressure; // Use pre-calculated unit label (hPa or inHg)
        return units.value === 'imperial' ? pascalsToInHg(value) : pascalsToHpa(value);
      case 'altitude':
        unitLabel = displayUnits.value.altitude; // Use pre-calculated unit label (m or ft)
        return units.value === 'imperial' ? metersToFeet(value) : value;
      case 'humidity':
      case 'moisture':
        unitLabel = '%';
        return value; // No conversion needed for percentage values
      default:
        unitLabel = ''; // Should not happen with dropdown
        return value;
    }
  });

  // Capitalize metric name for label (e.g., "Temperature")
  const metricName = metricKey.charAt(0).toUpperCase() + metricKey.slice(1);

  // Define distinct colors for different metrics
  const colors = {
    temperature: '#4a90e2', // Blue
    humidity: '#50e3c2',    // Teal
    pressure: '#f5a623',    // Orange
    heatIndex: '#d0021b',   // Red
    moisture: '#7ed321',    // Green
    altitude: '#9013fe'     // Purple
  };
  const color = colors[metricKey] || '#777777'; // Default grey if key not found

  return {
    labels: chartHistory.timestamps, // Use the timestamps array
    datasets: [
      {
        label: `${metricName} (${unitLabel})`, // Dynamic label
        backgroundColor: color,           // Dynamic color
        borderColor: color,               // Dynamic color
        data: dataPoints,                 // Processed data points
        tension: 0.1,
        pointRadius: 1,
        fill: false,
      },
    ],
  };
});

// *** Make chartOptions computed property dynamic based on selectedChartMetric ***
const chartOptions = computed(() => {
  const metricKey = selectedChartMetric.value;
  let yAxisLabel = metricKey.charAt(0).toUpperCase() + metricKey.slice(1);
  let beginAtZero = false; // Default: don't force y-axis to start at 0

  // Set Y-axis label based on metric and units, decide if starting at zero makes sense
  switch (metricKey) {
    case 'temperature': yAxisLabel += ` (${displayUnits.value.temp})`; break;
    case 'heatIndex': yAxisLabel += ` (${displayUnits.value.temp})`; break;
    case 'pressure': yAxisLabel += ` (${displayUnits.value.pressure})`; break;
    case 'altitude': yAxisLabel += ` (${displayUnits.value.altitude})`; break;
    case 'humidity':
      yAxisLabel += ' (%)';
      beginAtZero = true; // Percentages often start at 0
      break;
    case 'moisture':
      yAxisLabel += ' (%)';
      beginAtZero = true; // Percentages often start at 0
      break;
  }

  return {
    responsive: true,
    maintainAspectRatio: false,
    animation: { duration: 0 }, // Fast updates for live data
    scales: {
      x: {
        type: 'timeseries',
        time: {
          unit: 'second', // Base unit
          tooltipFormat: 'PPpp', // Detailed tooltip time
          displayFormats: { second: 'HH:mm:ss', minute: 'HH:mm' }
        },
        ticks: { source: 'auto', maxTicksLimit: 10 }, // Auto ticks, limit quantity
        title: { display: true, text: 'Time' }
      },
      y: {
        beginAtZero: beginAtZero, // Dynamic based on metric
        title: { display: true, text: yAxisLabel } // Dynamic Y-axis title
      }
    },
    plugins: {
      legend: { display: true, position: 'top' }, // Show legend now we have dynamic labels
      title: { display: false } // Keep using the H3 for main title
    }
  };
});


</script>

<style scoped>
.station-view {
  padding: 20px;
  max-width: 1200px;
  margin: 0 auto;
}

.unit-toggle {
  margin-bottom: 20px;
  text-align: right;
}

.unit-toggle label {
  margin-right: 10px;
  font-weight: bold;
}

.unit-toggle button {
  padding: 5px 10px;
  margin-left: 5px;
  border: 1px solid #ccc;
  background-color: #f0f0f0;
  cursor: pointer;
  border-radius: 4px;
}

.unit-toggle button.active {
  background-color: #4a90e2;
  color: white;
  border-color: #4a90e2;
}

.loading,
.error {
  text-align: center;
  padding: 20px;
  font-size: 1.1em;
}

.error {
  color: red;
  background-color: #ffebee;
  border: 1px solid red;
  border-radius: 4px;
}

.live-data-grid {
  display: grid;
  /* Adjusted minmax for potentially fitting more cards including AI/Timestamp */
  grid-template-columns: repeat(auto-fit, minmax(170px, 1fr));
  gap: 15px;
  margin-bottom: 30px;
}

.ai-card {
  /* Adjust span based on typical screen sizes if needed */
  grid-column: span 2;
  /* Make AI card wider if possible */
}

.timestamp-card {
  background-color: #f8f9fa;
  border-radius: 8px;
  padding: 15px 20px;
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
  text-align: center;
  border: 1px solid #e9ecef;
}

.timestamp-card h4 {
  margin: 0 0 8px 0;
  color: #6c757d;
  font-size: 0.9em;
  text-transform: uppercase;
}

.timestamp-card p {
  font-size: 1em;
  color: #495057;
  font-weight: 500;
  margin: 0;
}

@media (max-width: 900px) {

  /* Adjust breakpoint as needed */
  .ai-card {
    grid-column: span 2;
    /* Maybe keep it wider on medium screens */
  }
}

@media (max-width: 600px) {
  .ai-card {
    grid-column: span 1;
    /* Stack on small screens */
  }

  .timestamp-card {
    grid-column: span 1;
    /* Stack on small screens */
  }
}

.chart-container {
  background-color: #fff;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
  /* Increased height slightly to accommodate selector? Adjust as needed */
  min-height: 400px;
  display: flex;
  /* Use flexbox for better control */
  flex-direction: column;
  /* Stack title, selector, chart vertically */
}

.chart-container h3 {
  margin-top: 0;
  margin-bottom: 10px;
  /* Space below title */
  text-align: center;
  color: #333;
}

.chart-metric-selector {
  margin-bottom: 15px;
  /* Space below selector */
  text-align: center;
  /* Center selector elements */
}

.chart-metric-selector label {
  margin-right: 8px;
  font-weight: bold;
  color: #555;
}

.chart-metric-selector select {
  padding: 5px 8px;
  border: 1px solid #ccc;
  border-radius: 4px;
  background-color: #fff;
  cursor: pointer;
}

/* Ensure LineChart component takes remaining space */
.chart-container> :deep(canvas),
/* Target canvas inside LineChart */
.chart-container>p

/* Target "Collecting data..." text */
  {
  flex-grow: 1;
  /* Allow chart/placeholder to fill available space */
  min-height: 250px;
  /* Ensure it doesn't collapse */
}

.chart-container {
  background-color: #fff;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);

  /* *** 1. SET A FIXED HEIGHT *** */
  height: 400px;
  /* Adjust this value as needed */
  /* min-height: 400px; */
  /* Replace min-height with height */

  display: flex;
  flex-direction: column;
  /* position: relative; */
  /* Keep if needed, but maybe not required with fixed height */
}

.chart-container h3,
.chart-container .chart-metric-selector {
  /* *** 2. Prevent title/selector from growing/shrinking vertically *** */
  flex-shrink: 0;
}

/* *** 3. Target the chart element/placeholder to grow *** */
/* This targets the <LineChart> component instance or the <p> tag */
.chart-container> :nth-last-child(1):not(h3):not(.chart-metric-selector) {
  flex-grow: 1;
  /* Allow it to take up remaining space */

  /* *** 4. CRUCIAL: Prevent flex item from expanding container *** */
  min-height: 0;

  /* Add position relative for Chart.js internal calculations if needed*/
  position: relative;
}
</style>
