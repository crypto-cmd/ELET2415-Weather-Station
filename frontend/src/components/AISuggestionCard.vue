<template>
  <div class="ai-suggestion-card">
    <h4>💡Weather Insight</h4>
    <p v-if="suggestion" v-html="suggestion"></p> <p v-else>Calculating suggestion...</p>
    <small>Based on current station conditions ({{ formattedTime }})</small>
  </div>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
  weatherData: { type: Object, default: () => ({}) }
});

// Helper to get the hour from the timestamp
const currentHour = computed(() => {
    if (!props.weatherData?.timestamp) return null;
    return new Date(props.weatherData.timestamp * 1000).getHours();
});

const formattedTime = computed(() => {
     if (!props.weatherData?.timestamp) return 'No timestamp';
     // More concise time format for the card footer
     try {
        return new Date(props.weatherData.timestamp * 1000).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
     } catch(e) {
         return 'Invalid time';
     }
});

const suggestion = computed(() => {
  const data = props.weatherData; // Shorter alias

  // Check if essential data is available
  if (data?.heatIndex === null || data?.heatIndex === undefined ||
    data?.temperature === null || data?.temperature === undefined ||
    data?.humidity === null || data?.humidity === undefined) {
    return "Waiting for complete weather data...";
  }

  const temp = data.temperature;
  const humidity = data.humidity;
  const heatIndex = data.heatIndex;
  const moisture = data.moisture;
  const hour = currentHour.value;

  let suggestions = []; // Collect potential suggestions

  // --- Prioritized Conditions ---

  // 1. Heat Index (Safety First)
  if (heatIndex > 40) {
    suggestions.push("⚠️ <b>Extreme Heat Alert!</b> It feels dangerously hot. Crucial to stay hydrated, avoid strenuous outdoor activity completely, and seek cool environments. Check on neighbours, especially the elderly.");
  } else if (heatIndex > 35) {
    suggestions.push("🔥 <b>Very Hot Conditions.</b> Limit time outdoors, especially during midday (11 AM - 4 PM). Drink plenty of fluids (water!), wear loose, light-coloured clothing, and use sun protection (hat, sunscreen).");
  } else if (heatIndex > 30) {
    suggestions.push("☀️ It feels quite warm and potentially uncomfortable due to heat index. Stay hydrated, take breaks in the shade if working outdoors, and consider lighter activities.");
  } else if (heatIndex > 27) {
    suggestions.push("🌡️ Feeling warm. Remember to drink water. Good conditions for most activities, but be mindful of the heat building up later.");
  }

  // 2. Temperature (Cold Conditions - less likely in Kingston, but included for completeness)
  if (temp < 10) {
    suggestions.push("🥶 <b>Very Cold!</b> Unusually chilly for Kingston. Bundle up significantly if you must go out.");
  } else if (temp < 15) {
    suggestions.push("쌀쌀 Cool temperatures. A sweater or light jacket is recommended, especially if you feel the cold easily.");
  } else if (temp < 20 && heatIndex <= 27) { // Add condition to avoid overlap with heat index advice
    suggestions.push("쾌적 Mild temperatures. Comfortable for outdoor activities. Maybe a light layer needed if breezy (wind data not available).");
  }

  // 3. Humidity (Comfort & Other Effects) - Add only if not already covered by strong heat index message
  if (humidity > 85 && heatIndex <= 35) { // Avoid redundancy with extreme heat warnings
    suggestions.push("💧 High humidity making it feel very muggy and 'sticky'. May feel warmer than the actual temperature indicates. Laundry might take longer to dry.");
  } else if (humidity > 70 && heatIndex <= 30) {
    suggestions.push("☁️ Quite humid. Can make mild temperatures feel cooler or warm temperatures feel more uncomfortable.");
  } else if (humidity < 30) {
    suggestions.push("🌵 Air is quite dry. May cause dry skin or static. Drink water!");
  }

  // 4. Soil Moisture (Specific Use Case)
  if (moisture !== null && moisture !== undefined) {
    if (moisture < 18) {
      suggestions.push("🏜️ Soil appears very dry. Watering is likely needed for most plants unless they are drought-tolerant.");
    } else if (moisture < 30) {
      suggestions.push("🌱 Soil moisture is getting low. Consider watering garden plants, lawns, or potted plants soon, especially if no rain is forecast.");

    } else if (moisture > 80) {
      suggestions.push("🌧️ Soil seems quite saturated. Hold off on watering unless plants specifically require wet conditions.");
    }
  }

  // 5. Time-based context (Simple Morning Context)
  if (hour !== null) {
    if (hour >= 5 && hour < 12 && suggestions.length === 0) { // Only add if no other major suggestion
      suggestions.push("☀️ Good morning! Weather seems moderate right now. Enjoy the start to your day in Kingston.");
    } else if (hour >= 12 && hour < 17 && suggestions.length === 0) {
      suggestions.push("🌤️ Conditions seem pleasant this afternoon. Good time for outdoor activities.");
    } else if (hour >= 17 && suggestions.length === 0) {
      suggestions.push("🌙 As evening approaches, conditions remain moderate.");
    }
  }


  // --- Compile Final Suggestion ---
  if (suggestions.length > 0) {
    // Return the highest priority suggestion (the first one added based on the logic above)
    return suggestions[0];
  } else {
    // Default message if no specific conditions met
    return "✅ Weather conditions seem moderate and comfortable. Enjoy your day in Kingston!";
  }
});
// --- End AI Simulation ---

</script>

<style scoped>
.ai-suggestion-card {
  background-color: #eef2f7;
  /* Lighter blue background */
  border-radius: 8px;
  padding: 15px 20px;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.05);
  border-left: 5px solid #4a90e2;
  /* Accent color */
}

.ai-suggestion-card h4 {
  margin: 0 0 10px 0;
  color: #345a80;
  font-size: 1em;
}

.ai-suggestion-card p {
  margin: 0 0 5px 0;
  color: #333;
  font-size: 0.95em;
  line-height: 1.4;
}

.ai-suggestion-card small {
  color: #777;
  font-style: italic;
}
</style>
