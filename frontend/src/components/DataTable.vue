<template>
  <div class="data-table-container">
    <table v-if="items.length > 0">
      <thead>
        <tr>
          <th>Timestamp</th>
          <th>Temp ({{ displayUnits.temp }})</th>
          <th>Humidity (%)</th>
          <th>Pressure ({{ displayUnits.pressure }})</th>
          <th>Altitude ({{ displayUnits.altitude }})</th>
          <th>Heat Index ({{ displayUnits.temp }})</th>
          <th>Moisture (%)</th>
          </tr>
      </thead>
      <tbody>
        <tr v-for="item in processedItems" :key="item.id || item.timestamp">
          <td>{{ formatTimestamp(item.timestamp) }}</td>
          <td>{{ item.temperature?.toFixed(1) ?? 'N/A' }}</td>
          <td>{{ item.humidity?.toFixed(0) ?? 'N/A' }}</td>
          <td>{{ item.pressure?.toFixed(1) ?? 'N/A' }}</td>
          <td>{{ item.altitude?.toFixed(1) ?? 'N/A' }}</td>
          <td>{{ item.heatIndex?.toFixed(1) ?? 'N/A' }}</td>
           <td>{{ item.moisture?.toFixed(0) ?? 'N/A' }}</td>
          </tr>
      </tbody>
    </table>
    <p v-else>No data available for the selected range.</p>
  </div>
</template>

<script setup>
import { computed } from 'vue';

import {useAppStore} from '@/store/appStore'; // Assuming you have a store for global state
const {formatTimestamp, celsiusToFahrenheit, pascalsToHpa, pascalsToInHg, metersToFeet} = useAppStore(); // Assuming you have a store for global state
const props = defineProps({
  items: { type: Array, default: () => [] },
  units: { type: String, default: 'metric' } // 'metric' or 'imperial'
});

const displayUnits = computed(() => ({
    temp: props.units === 'metric' ? '°C' : '°F',
    pressure: props.units === 'metric' ? 'hPa' : 'inHg',
    altitude: props.units === 'metric' ? 'm' : 'ft',
}));

// Process items for display including unit conversion
const processedItems = computed(() => {
    return props.items.map(item => {
        let processed = { ...item }; // Clone item

        if (props.units === 'imperial') {
            processed.temperature = celsiusToFahrenheit(item.temperature);
            processed.heatIndex = celsiusToFahrenheit(item.heatIndex);
            processed.pressure = pascalsToInHg(item.pressure);
            processed.altitude = metersToFeet(item.altitude);
        } else { // Metric
            processed.pressure = pascalsToHpa(item.pressure);
            // Temperature, HeatIndex, Altitude are assumed metric from API
        }
        return processed;
    });
});

</script>

<style scoped>
.data-table-container {
  overflow-x: auto; /* Allow horizontal scrolling on small screens */
  max-height: 500px; /* Limit height and allow vertical scroll */
  overflow-y: auto;
  background-color: #fff;
  border-radius: 8px;
  box-shadow: 0 1px 4px rgba(0,0,0,0.1);
}
table {
  width: 100%;
  border-collapse: collapse;
}
th, td {
  border: 1px solid #ddd;
  padding: 10px 12px;
  text-align: right;
  white-space: nowrap; /* Prevent text wrapping */
}
th {
  background-color: #f2f2f2;
  font-weight: bold;
  position: sticky; /* Make header sticky */
  top: 0; /* Stick to the top of the container */
  z-index: 1;
}
tbody tr:nth-child(even) {
  background-color: #f9f9f9;
}
tbody tr:hover {
  background-color: #e6f7ff;
}
td:first-child, th:first-child {
    text-align: left; /* Align timestamp left */
}
</style>
