<template>
  <div class="analysis-view">
    <h2>Historical Data Analysis</h2>

    <div class="unit-toggle">
      <label>Units: </label>
      <button @click="units = 'metric'" :class="{ active: units === 'metric' }">Metric (°C, hPa)</button>
      <button @click="units = 'imperial'" :class="{ active: units === 'imperial' }">Imperial (°F, inHg)</button>
    </div>

    <DataFilters @fetch-data="handleFetchData" />

    <div v-if="loading" class="loading">Loading historical data...</div>
    <div v-if="error" class="error">Error fetching data: {{ error }}</div>

    <div v-if="historicalData.length > 0">
      <div class="chart-container">
        <h3>Trend for Selected Period</h3>
        <div class="metric-selector">
          <label>Plot: </label>
          <select v-model="selectedMetric">
            <option value="temperature">Temperature</option>
            <option value="humidity">Humidity</option>
            <option value="pressure">Pressure</option>
            <option value="altitude">Altitude</option>
            <option value="heatIndex">Heat Index</option>
            <option value="moisture">Moisture</option>
          </select>
        </div>
        <LineChart v-if="chartData.labels.length > 1" :chart-data="chartData" :chart-options="chartOptions" />
        <p v-else>Not enough data points to plot.</p>
      </div>

      <DataTable :items="historicalData" :units="units" />
    </div>
    <div v-else-if="!loading && !error && hasFetched">
      <p class="no-data-message">No data found for the selected time range.</p>
    </div>
  </div>
</template>

<script setup>
import { ref, computed } from 'vue';
import { useAppStore } from '@/store/appStore'; // Assuming you have a store for global state
import DataFilters from '@/components/DataFilters.vue';
import DataTable from '@/components/DataTable.vue';
import LineChart from '@/components/LineChart.vue';

const { getDataInRange, celsiusToFahrenheit, pascalsToHpa, pascalsToInHg, metersToFeet } = useAppStore(); // Assuming you have a store for global state
const historicalData = ref([]);
const loading = ref(false);
const error = ref(null);
const units = ref('metric'); // 'metric' or 'imperial'
const selectedMetric = ref('temperature'); // Default metric to plot
const hasFetched = ref(false); // Track if a fetch has been attempted

const handleFetchData = async ({ start, end }) => {
  loading.value = true;
  error.value = null;
  historicalData.value = []; // Clear previous data
  hasFetched.value = true; // Mark that a fetch was initiated
  console.log(`AnalysisView: Fetching data from ${start} to ${end}`);
  try {
    const data = await getDataInRange(start, end);
    // Sort data by timestamp just in case API doesn't guarantee order
    historicalData.value = data.sort((a, b) => a.timestamp - b.timestamp);
    console.log(`AnalysisView: Received ${data.length} data points.`);
  } catch (err) {
    error.value = err.message || 'Failed to fetch historical data';
    console.error("AnalysisView: Fetch error:", err);
  } finally {
    loading.value = false;
  }
};

// Computed property for Chart.js data structure based on selected metric
const chartData = computed(() => {
  if (!historicalData.value || historicalData.value.length === 0) {
    return { labels: [], datasets: [] };
  }

  const labels = historicalData.value.map(item => new Date(item.timestamp * 1000));
  let dataPoints = [];
  let unitLabel = '';

  // Dynamically select data based on selectedMetric and apply unit conversion
  historicalData.value.forEach(item => {
    let value = item[selectedMetric.value];
    switch (selectedMetric.value) {
      case 'temperature':
      case 'heatIndex':
        unitLabel = units.value === 'imperial' ? '°F' : '°C';
        value = units.value === 'imperial' ? celsiusToFahrenheit(value) : value;
        break;
      case 'pressure':
        unitLabel = units.value === 'imperial' ? 'inHg' : 'hPa';
        value = units.value === 'imperial' ? pascalsToInHg(value) : pascalsToHpa(value);
        break;
      case 'altitude':
        unitLabel = units.value === 'imperial' ? 'ft' : 'm';
        value = units.value === 'imperial' ? metersToFeet(value) : value;
        break;
      case 'humidity':
      case 'moisture':
        unitLabel = '%';
        // No conversion needed
        break;
      default:
        unitLabel = ''; // Or some default/error indicator
        value = null;
    }
    dataPoints.push(value);
  });


  return {
    labels: labels,
    datasets: [
      {
        label: `${selectedMetric.value.charAt(0).toUpperCase() + selectedMetric.value.slice(1)} (${unitLabel})`,
        backgroundColor: '#007bff',
        borderColor: '#007bff',
        data: dataPoints,
        tension: 0.1,
        pointRadius: historicalData.value.length > 100 ? 0 : 2, // Hide points if too many
        fill: false,
      },
    ],
  };
});

// Chart options (can be customized further)
const chartOptions = computed(() => ({
  responsive: true,
  maintainAspectRatio: false,
  scales: {
    x: {
      type: 'timeseries',
      time: {
        unit: 'minute', // Auto adapt unit based on range (Chart.js does this well)
        tooltipFormat: 'PPpp',
        displayFormats: { // Define formats for different time units
          minute: 'HH:mm',
          hour: 'MMM d HH:mm',
          day: 'MMM d',
          month: 'MMM yyyy'
        }
      },
      title: { display: true, text: 'Time' }
    },
    y: {
      beginAtZero: selectedMetric.value === 'humidity' || selectedMetric.value === 'moisture', // Start at zero for % values
      title: {
        display: true,
        text: `${selectedMetric.value.charAt(0).toUpperCase() + selectedMetric.value.slice(1)} (${chartData.value.datasets[0]?.label.split('(')[1]?.split(')')[0] ?? ''})`
      }
    }
  },
  plugins: {
    legend: { display: true, position: 'bottom' },
    title: { display: false } // Use the h3 title
  },
  interaction: { // Enable zooming and panning
    mode: 'index',
    intersect: false,
  },
  // Consider adding zoom plugin if needed: chartjs-plugin-zoom
}));


</script>

<style scoped>
.analysis-view {
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
.error,
.no-data-message {
  text-align: center;
  padding: 20px;
  font-size: 1.1em;
  margin-top: 20px;
}

.error {
  color: red;
  background-color: #ffebee;
  border: 1px solid red;
  border-radius: 4px;
}

.no-data-message {
  color: #666;
  background-color: #f9f9f9;
  border: 1px solid #eee;
  border-radius: 4px;
}

.chart-container {
  background-color: #fff;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
  height: 400px;
  /* Fixed height for the chart area */
  margin-bottom: 30px;
  /* Space before table */
  position: relative;
  /* For positioning metric selector */
}

.chart-container h3 {
  margin-top: 0;
  text-align: center;
  color: #333;
}

.metric-selector {
  position: absolute;
  top: 15px;
  right: 20px;
  z-index: 10;
}

.metric-selector label {
  margin-right: 5px;
  font-size: 0.9em;
}

.metric-selector select {
  padding: 3px 5px;
  border-radius: 4px;
  border: 1px solid #ccc;
}
</style>
