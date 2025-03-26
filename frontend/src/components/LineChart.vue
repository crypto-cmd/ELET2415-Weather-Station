<template>
  <Line :data="chartData" :options="chartOptions" />
</template>

<script setup>
import { Line } from 'vue-chartjs';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  TimeSeriesScale // Import TimeScale for time-based X-axis
} from 'chart.js';
import 'chartjs-adapter-date-fns'; // Import the date adapter

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  TimeSeriesScale // Register TimeScale
);

const props = defineProps({
  chartData: {
    type: Object,
    required: true,
    default: () => ({ labels: [], datasets: [] }) // Ensure defaults
  },
  chartOptions: {
    type: Object,
    default: () => ({
      responsive: true,
      maintainAspectRatio: false,
      scales: {
        x: {
          type: 'timeseries', // Use timeseries scale
          time: {
            unit: 'minute', // Adjust based on data density
             tooltipFormat: 'PPpp', // Example format from date-fns
             displayFormats: {
                 minute: 'HH:mm',
                 hour: 'HH:mm',
                 day: 'MMM d'
             }
          },
          title: {
            display: true,
            text: 'Time'
          }
        },
        y: {
          beginAtZero: false, // Adjust as needed
          title: {
            display: true,
            text: 'Value'
          }
        }
      },
      plugins: {
        legend: {
          position: 'top',
        },
        title: {
          display: true,
          text: 'Weather Trend'
        }
      }
    })
  }
});

</script>
