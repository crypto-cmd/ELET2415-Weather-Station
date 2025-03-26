<template>
  <div class="about-view">
    <h2>About This Weather Station Project</h2>

    <section>
      <h3>Project Overview</h3>
      <p>
        This application serves as the frontend interface for a custom-built embedded weather station device for the
        course ELET2415
        It displays real-time sensor data and provides tools for analyzing historical weather trends specific to its
        location
        (currently monitoring conditions in <strong>Kingston, Jamaica</strong>).
      </p>
    </section>

    <section>
      <h3>Key Features</h3>
      <ul>
        <li><strong>Real-time Data Display:</strong> Live updates from the weather station sensors refreshed every
          second on the "Station" page.</li>
        <li><strong>Data Visualization:</strong> Dynamic charts showing live trends and historical data patterns.</li>
        <li><strong>Historical Analysis:</strong> Ability to select date ranges and view past weather data in both chart
          and table formats on the "Analysis" page.</li>
        <li><strong>Unit Conversion:</strong> Toggle between Metric (°C, hPa, m) and Imperial (°F, inHg, ft) units.</li>
        <li><strong>Suggestions:</strong> Simple context-aware suggestions based on current weather conditions.</li>
        <li><strong>Auto-Updating Interface:</strong> Charts and data cards update automatically without requiring
          manual refreshes.</li>
        <li><strong>API Integration:</strong> Communicates with the backend API (built with Flask) to fetch sensor data.
        </li>
        <li><strong>Frontend Logging:</strong> Includes a basic console log viewer in the footer for debugging purposes.
        </li>
      </ul>
    </section>

  </div>
</template>

<script setup>
import { ref } from 'vue';

// Get current date for display
const currentDate = ref(new Date().toLocaleDateString('en-JM', { // Use Jamaican locale formatting
  weekday: 'long', year: 'numeric', month: 'long', day: 'numeric',
  hour: '2-digit', minute: '2-digit', timeZoneName: 'short' // Include time and timezone
}));

</script>

<style scoped>
.about-view {
  padding: 20px 30px;
  max-width: 900px;
  margin: 20px auto;
  background-color: #fff;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

h2 {
  text-align: center;
  color: #1c3144;
  margin-bottom: 30px;
}

h3 {
  color: #3a506b;
  border-bottom: 2px solid #e0e0e0;
  padding-bottom: 5px;
  margin-top: 30px;
  margin-bottom: 15px;
}

p {
  line-height: 1.6;
  color: #333;
}

ul {
  list-style-type: disc;
  padding-left: 25px;
  color: #333;
}

li {
  margin-bottom: 8px;
}

strong {
  color: #0056b3;
  /* Make highlighted text stand out */
}

.tech-stack {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 20px;
  margin-top: 15px;
}

.tech-stack h4 {
  margin-bottom: 10px;
  color: #345a80;
}

section:last-of-type {
  margin-top: 40px;
  text-align: center;
  font-size: 0.9em;
  color: #555;
}

section:last-of-type strong {
  color: #333;
  /* Standard color for name */
}

section small {
  display: block;
  margin-top: 15px;
  color: #777;
}
</style>
