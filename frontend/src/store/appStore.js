import { defineStore } from "pinia";
import { ref } from "vue";
import { useToast } from "vue-toastify";
const BASE_URL = "http://localhost:9000/api";
export const useAppStore = defineStore(
  "app",
  () => {
    /*
    The composition API way of defining a Pinia store
    ref() s become state properties
    computed() s become getters
    function() s become actions
    */

    async function getLatestData() {
      console.log("Fetching latest data...");
      try {
        const response = await fetch(`${BASE_URL}/data`);
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const result = await response.json();
        if (result.status === "success") {
          console.log("Latest data received:", result.data);
          return result.data;
        } else {
          throw new Error("API returned status other than success");
        }
      } catch (error) {
        console.error("Error fetching latest data:", error);
        throw error; // Re-throw to allow components to handle it
      }
    }
    async function getDataInRange(startTimestamp, endTimestamp) {
      console.log(
        `Workspaceing data from ${startTimestamp} to ${endTimestamp}...`
      );
      try {
        const response = await fetch(
          `${BASE_URL}/data/${startTimestamp}/${endTimestamp}`
        );
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const result = await response.json();
        if (result.status === "success") {
          console.log(`Data received for range: ${result.data.length} items`);
          return result.data;
        } else {
          throw new Error("API returned status other than success");
        }
      } catch (error) {
        console.error("Error fetching data in range:", error);
        throw error; // Re-throw to allow components to handle it
      }
    }

    // Conversions
    function celsiusToFahrenheit(celsius) {
      if (celsius === null || celsius === undefined) return null;
      return (celsius * 9) / 5 + 32;
    }

    function fahrenheitToCelsius(fahrenheit) {
      if (fahrenheit === null || fahrenheit === undefined) return null;
      return ((fahrenheit - 32) * 5) / 9;
    }

    function pascalsToHpa(pascals) {
      if (pascals === null || pascals === undefined) return null;
      return pascals / 100;
    }

    function pascalsToInHg(pascals) {
      if (pascals === null || pascals === undefined) return null;
      return pascals * 0.0002953;
    }

    // Add more conversions as needed (e.g., meters to feet for altitude)
    function metersToFeet(meters) {
      if (meters === null || meters === undefined) return null;
      return meters * 3.28084;
    }

    function formatTimestamp(timestamp) {
      if (timestamp === null || timestamp === undefined) return "N/A";
      try {
        const date = new Date(timestamp * 1000); // JS Date expects milliseconds
        return date.toLocaleString(); // Adjust options as needed for specific format
      } catch (e) {
        return "Invalid Date";
      }
    }
    // ACTIONS

    return {
      // EXPORTS
      getLatestData,
      getDataInRange,

      // CONVERSIONS
      celsiusToFahrenheit,
      fahrenheitToCelsius,
      pascalsToHpa,
      pascalsToInHg,
      metersToFeet,
      formatTimestamp,
    };
  },
  { persist: true }
);
