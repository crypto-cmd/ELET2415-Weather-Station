<template>
  <div class="weather-card">
    <h4>{{ title }}</h4>
    <p class="value">{{ displayValue }} <span class="unit">{{ displayUnit }}</span></p>
    <p v-if="secondaryValue !== null" class="secondary-value">
      {{ secondaryDisplayValue }} <span class="unit">{{ secondaryUnit }}</span>
    </p>
  </div>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
  title: { type: String, required: true },
  value: { type: Number, default: null },
  unit: { type: String, required: true },
  // Optional secondary display (e.g., for converted units)
  secondaryValue: { type: Number, default: null },
  secondaryUnit: { type: String, default: '' },
  precision: { type: Number, default: 1 } // Number of decimal places
});

const displayValue = computed(() => {
  if (props.value === null || props.value === undefined) {
    return '--';
  }
  return props.value.toFixed(props.precision);
});

const secondaryDisplayValue = computed(() => {
  if (props.secondaryValue === null || props.secondaryValue === undefined) {
    return '--';
  }
  return props.secondaryValue.toFixed(props.precision);
});

const displayUnit = computed(() => props.unit);

</script>

<style scoped>
.weather-card {
  background-color: #fff;
  border-radius: 8px;
  padding: 15px 20px;
  box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
  text-align: center;
  min-width: 150px;
  transition: transform 0.2s ease-in-out;
}

.weather-card:hover {
  transform: translateY(-3px);
}

.weather-card h4 {
  margin: 0 0 10px 0;
  color: #555;
  font-size: 0.9em;
  text-transform: uppercase;
}

.weather-card .value {
  font-size: 1.8em;
  font-weight: bold;
  margin: 0;
  color: #333;
}

.weather-card .unit {
  font-size: 0.7em;
  margin-left: 3px;
  color: #777;
}

.weather-card .secondary-value {
  font-size: 0.9em;
  margin-top: 5px;
  color: #666;
}
</style>
