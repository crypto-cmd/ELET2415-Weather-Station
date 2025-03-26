<template>
  <div class="data-filters">
    <label for="startDate">Start:</label>
    <input type="datetime-local" id="startDate" v-model="localStartDate">

    <label for="endDate">End:</label>
    <input type="datetime-local" id="endDate" v-model="localEndDate">

    <button @click="emitFetchData">Fetch Data</button>
    <p v-if="error" class="error-message">{{ error }}</p>
  </div>
</template>

<script setup>
import { ref, watch } from 'vue';

const emit = defineEmits(['fetch-data']);

// Use refs for local component state tied to inputs
const localStartDate = ref(getDefaultStartDate()); // Default to 1 hour ago
const localEndDate = ref(getCurrentDateTimeLocal()); // Default to now
const error = ref('');

function getCurrentDateTimeLocal() {
  const now = new Date();
  now.setMinutes(now.getMinutes() - now.getTimezoneOffset()); // Adjust for local timezone
  return now.toISOString().slice(0, 16); // Format for datetime-local
}

function getDefaultStartDate() {
    const oneHourAgo = new Date();
    oneHourAgo.setHours(oneHourAgo.getHours() - 1);
    oneHourAgo.setMinutes(oneHourAgo.getMinutes() - oneHourAgo.getTimezoneOffset()); // Adjust for local timezone
    return oneHourAgo.toISOString().slice(0, 16);
}

// Function to convert local datetime string to Unix timestamp (seconds)
function getTimestamp(dateTimeLocal) {
    if (!dateTimeLocal) return null;
    // Important: Date parsing from input 'datetime-local' assumes local time.
    // Date.parse() or new Date() will use this local time.
    // We need Unix timestamp (seconds), which is UTC based.
    const date = new Date(dateTimeLocal);
    return Math.floor(date.getTime() / 1000);
}


function emitFetchData() {
  error.value = ''; // Clear previous errors
  const start = getTimestamp(localStartDate.value);
  const end = getTimestamp(localEndDate.value);

  if (!start || !end) {
      error.value = 'Please select both start and end dates.';
      return;
  }

  if (start >= end) {
      error.value = 'Start date must be before end date.';
      return;
  }

  console.log(`Emitting fetch-data with range: ${start} to ${end}`);
  emit('fetch-data', { start, end });
}

// Optional: Watch for changes if you want immediate feedback or validation
// watch([localStartDate, localEndDate], () => {
//   // Add validation logic here if needed
// });

</script>

<style scoped>
.data-filters {
  display: flex;
  gap: 15px;
  align-items: center;
  padding: 15px;
  background-color: #f9f9f9;
  border-radius: 8px;
  margin-bottom: 20px;
  flex-wrap: wrap; /* Allow wrapping on smaller screens */
}
.data-filters label {
  margin-right: 5px;
  font-weight: bold;
}
.data-filters input[type="datetime-local"] {
  padding: 8px;
  border: 1px solid #ccc;
  border-radius: 4px;
}
.data-filters button {
  padding: 8px 15px;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: background-color 0.2s;
}
.data-filters button:hover {
  background-color: #45a049;
}
.error-message {
    color: red;
    font-size: 0.9em;
    margin-left: 10px;
}
</style>
