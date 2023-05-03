#include "esphome.h"
#include <ctime>

int lineNum = 1;

class SensorMeterHeating : public PollingComponent {
public:
  Sensor *heat_usage = new Sensor();
  const String HEAT_USAGE_PREFIX = "6.8(";
  const String HEAT_USAGE_SUFFIX = "*MWh)6.26(";

  Sensor *volume_usage = new Sensor();
  const String VOLUME_USAGE_PREFIX = "6.26(";
  const String VOLUME_USAGE_SUFFIX = "*m3)9.21(";

  Sensor *ownership_number= new Sensor();
  const String OWNERSHIP_NUMBER_PREFIX = "*m3)9.21(";
  const String OWNERSHIP_NUMBER_SUFFIX = ")";

  Sensor *volume_previous_year= new Sensor();
  const String VOLUME_PREVIOUS_YEAR_PREFIX = "6.26*01(";
  const String VOLUME_PREVIOUS_YEAR_SUFFIX = "*m3)6.8*01(";

  Sensor *heat_previous_year= new Sensor();
  const String HEAT_PREVIOUS_YEAR_PREFIX = "*m3)6.8*01(";
  const String HEAT_PREVIOUS_YEAR_SUFFIX = "*MWh)";

  Sensor *error_number= new Sensor();
  const String ERROR_NUMBER_PREFIX = "F(";
  const String ERROR_NUMBER_SUFFIX = ")9.20(";

  Sensor *device_number= new Sensor();
  const String DEVICE_NUMBER_PREFIX = ")9.20(";
  const String DEVICE_NUMBER_SUFFIX = ")6.35(";

  Sensor *measurement_period= new Sensor();
  const String MEASUREMENT_PERIOD_PREFIX = ")6.35(";
  const String MEASUREMENT_PERIOD_SUFFIX = "*m)";

  Sensor *power_max= new Sensor();
  const String POWER_MAX_PREFIX = "6.6(";
  const String POWER_MAX_SUFFIX = "*kW)6.6*01(";

  Sensor *power_max_previous_year= new Sensor();
  const String POWER_MAX_PREVIOUS_YEAR_PREFIX = "*kW)6.6*01(";
  const String POWER_MAX_PREVIOUS_YEAR_SUFFIX = "*kW)6.33(";

  Sensor *flowrate_max= new Sensor();
  const String FLOWRATE_MAX_PREFIX = "*kW)6.33(";
  const String FLOWRATE_MAX_SUFFIX = "*m3ph)9.4(";

  Sensor *flow_temperature_max= new Sensor();
  const String FLOW_TEMPERATURE_MAX_PREFIX = "*m3ph)9.4(";
  const String FLOW_TEMPERATURE_MAX_SUFFIX = "*C&";

  Sensor *return_temperature_max= new Sensor();
  const String RETURN_TEMPERATURE_MAX_PREFIX = "*C&";
  const String RETURN_TEMPERATURE_MAX_SUFFIX = "*C)";

  Sensor *operating_hours= new Sensor();
  const String OPERATING_HOURS_PREFIX = "6.31(";
  const String OPERATING_HOURS_SUFFIX = "*h)6.32(";

  Sensor *fault_hours= new Sensor();
  const String FAULT_HOURS_PREFIX = "*h)6.32(";
  const String FAULT_HOURS_SUFFIX = "*h)9.22(";

  Sensor *fault_hours_previous_year= new Sensor();
  const String FAULT_HOURS_PREVIOUS_YEAR_PREFIX = ")6.32*01(";
  const String FAULT_HOURS_PREVIOUS_YEAR_SUFFIX = "*h)6.36(";

  Sensor *flowrate_max_previous_year= new Sensor();
  const String FLOWRATE_MAX_PREVIOUS_YEAR_PREFIX = ")6.33*01(";
  const String FLOWRATE_MAX_PREVIOUS_YEAR_SUFFIX = "*m3ph)";

  Sensor *flow_temperature_max_previous_year= new Sensor();
  const String FLOW_TEMPERATURE_MAX_PREVIOUS_YEAR_PREFIX = "9.4*01(";
  const String FLOW_TEMPERATURE_MAX_PREVIOUS_YEAR_SUFFIX = "*C&";

  Sensor *return_temperature_max_previous_year= new Sensor();
  const String RETURN_TEMPERATURE_MAX_PREVIOUS_YEAR_PREFIX = "*C&";
  const String RETURN_TEMPERATURE_MAX_PREVIOUS_YEAR_SUFFIX = "*C)";

  Sensor *meter_date_time= new Sensor();
  const String METER_DATE_TIME_PREFIX = ")9.36(";
  const String METER_DATE_TIME_SUFFIX = ")9.24(";

  Sensor *measuring_range= new Sensor();
  const String MEASURING_RANGE_PREFIX = ")9.24(";
  const String MEASURING_RANGE_SUFFIX = "*m3ph)";

  Sensor *flow_hours= new Sensor();
  const String FLOW_HOURS_PREFIX = ")9.31(";
  const String FLOW_HOURS_SUFFIX = "*h)";

  SensorMeterHeating() : PollingComponent(60000) { }

  float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; }

  void setup() override { }

  void update() override {
    Serial.begin(300, SERIAL_7E1);
    Serial.setTimeout(1000);
    String line = "";

    // Send init message
    ESP_LOGD("reader", "Send init message");
    for (int i=0; i <= 40; i++) {
      Serial.write(0x00);
    }

    Serial.write("\x2F\x3F\x21\x0D\x0A");

    // Send request message
    // Seems to not be needed?
    //Serial.write("\x0D\x0A");

    Serial.flush();
    delay(1500);

    String IdMsg = Serial.readStringUntil('\n');
    ESP_LOGD("response", "Identification Message: %s", IdMsg.c_str());

    ESP_LOGD("reader", "Change Baudrate to 2400");
    Serial.begin(2400, SERIAL_7E1);

    // OBIS line 1
    line = readLine();

    float heatUsage = extractValue(line, HEAT_USAGE_PREFIX, HEAT_USAGE_SUFFIX);
    if (heatUsage != -1) {
      heat_usage->publish_state(heatUsage);
    }

    float volumeUsage = extractValue(line, VOLUME_USAGE_PREFIX, VOLUME_USAGE_SUFFIX);
    if (volumeUsage != -1) {
      volume_usage->publish_state(volumeUsage);
    }

    float ownershipNumber = extractValue(line, OWNERSHIP_NUMBER_PREFIX, OWNERSHIP_NUMBER_SUFFIX);
    if (ownershipNumber != -1) {
      ownership_number->publish_state(ownershipNumber);
    }

    // OBIS line 2
    line = readLine();

    float volumePreviousYear = extractValue(line, VOLUME_PREVIOUS_YEAR_PREFIX, VOLUME_PREVIOUS_YEAR_SUFFIX);
    if (volumePreviousYear != -1) {
      volume_previous_year->publish_state(volumePreviousYear);
    }

    float heatPreviousYear = extractValue(line, HEAT_PREVIOUS_YEAR_PREFIX, HEAT_PREVIOUS_YEAR_SUFFIX);
    if (heatPreviousYear != -1) {
      heat_previous_year->publish_state(heatPreviousYear);
    }

    // OBIS line 3
    line = readLine();

    float errorNumber = extractValue(line, ERROR_NUMBER_PREFIX, ERROR_NUMBER_SUFFIX);
    if (errorNumber != -1) {
      error_number->publish_state(errorNumber);
    }

    float deviceNumber = extractValue(line, DEVICE_NUMBER_PREFIX, DEVICE_NUMBER_SUFFIX);
    if (deviceNumber != -1) {
      device_number->publish_state(deviceNumber);
    }

    float measurementPeriod = extractValue(line, MEASUREMENT_PERIOD_PREFIX, MEASUREMENT_PERIOD_SUFFIX);
    if (measurementPeriod != -1) {
      measurement_period->publish_state(measurementPeriod);
    }

    // OBIS line 4
    line = readLine();

    float powerMax = extractValue(line, POWER_MAX_PREFIX, POWER_MAX_SUFFIX);
    if (powerMax != -1) {
      power_max->publish_state(powerMax);
    }

    float powerMaxPreviousYear = extractValue(line, POWER_MAX_PREVIOUS_YEAR_PREFIX, POWER_MAX_PREVIOUS_YEAR_SUFFIX);
    if (powerMaxPreviousYear != -1) {
      power_max_previous_year->publish_state(powerMaxPreviousYear);
    }

    float flowRateMax = extractValue(line, FLOWRATE_MAX_PREFIX, FLOWRATE_MAX_SUFFIX);
    if (flowRateMax != -1) {
      flowrate_max->publish_state(flowRateMax);
    }

    float flowTemperatureMax = extractValue(line, FLOW_TEMPERATURE_MAX_PREFIX, FLOW_TEMPERATURE_MAX_SUFFIX);
    if (flowTemperatureMax != -1) {
      flow_temperature_max->publish_state(flowTemperatureMax);
    }

    float returnTemperatureMax = extractValue(line, RETURN_TEMPERATURE_MAX_PREFIX, RETURN_TEMPERATURE_MAX_SUFFIX);
    if (returnTemperatureMax != -1) {
      return_temperature_max->publish_state(returnTemperatureMax);
    }

    // OBIS line 5
    line = readLine();

    float operatingHours = extractValue(line, OPERATING_HOURS_PREFIX, OPERATING_HOURS_SUFFIX);
    if (operatingHours != -1) {
      operating_hours->publish_state(operatingHours);
    }

    float faultHours = extractValue(line, FAULT_HOURS_PREFIX, FAULT_HOURS_SUFFIX);
    if (faultHours != -1) {
      fault_hours->publish_state(faultHours);
    }

    // OBIS line 6
    line = readLine();

    float faultHoursPreviousYear = extractValue(line, FAULT_HOURS_PREVIOUS_YEAR_PREFIX, FAULT_HOURS_PREVIOUS_YEAR_SUFFIX);
    if (faultHoursPreviousYear != -1) {
      fault_hours_previous_year->publish_state(faultHoursPreviousYear);
    }

    float flowrateMaxPreviousYear = extractValue(line, FLOWRATE_MAX_PREVIOUS_YEAR_PREFIX, FLOWRATE_MAX_PREVIOUS_YEAR_SUFFIX);
    if (flowrateMaxPreviousYear != -1) {
      flowrate_max_previous_year->publish_state(flowrateMaxPreviousYear);
    }

    // OBIS line 7-9 omitted, continue with line 10
    for (int i = 0; i <= 3; i++) {
      line = readLine();
    }

    float flowTemperatureMaxPreviousYear = extractValue(line, FLOW_TEMPERATURE_MAX_PREVIOUS_YEAR_PREFIX, FLOW_TEMPERATURE_MAX_PREVIOUS_YEAR_SUFFIX);
    if (flowTemperatureMaxPreviousYear != -1) {
      flow_temperature_max_previous_year->publish_state(flowTemperatureMaxPreviousYear);
    }

    float returnTemperatureMaxPreviousYear = extractValue(line, RETURN_TEMPERATURE_MAX_PREVIOUS_YEAR_PREFIX, RETURN_TEMPERATURE_MAX_PREVIOUS_YEAR_SUFFIX);
    if (returnTemperatureMaxPreviousYear != -1) {
      return_temperature_max_previous_year->publish_state(returnTemperatureMaxPreviousYear);
    }

    // OBIS line 11-14 omitted, continue with line 15
    for (int i = 0; i <= 4; i++) {
      line = readLine();
    }

    const size_t startIndex = line.indexOf(METER_DATE_TIME_PREFIX) + METER_DATE_TIME_PREFIX.length();
    if (startIndex != std::string::npos) {
      const size_t endIndex = line.indexOf(METER_DATE_TIME_SUFFIX, startIndex);
      if (endIndex != std::string::npos) {
        String value = line.substring(startIndex, endIndex);
        const char* cvalue = value.c_str();
        struct tm datetime = {0};
        if (strptime(cvalue, "%Y-%m-%d&%H:%M:%S", &datetime) != nullptr) {
          float meterDateTime = static_cast<float>(mktime(&datetime));
          meter_date_time->publish_state(meterDateTime);
        }
      }
    }

    float measuringRange = extractValue(line, MEASURING_RANGE_PREFIX, MEASURING_RANGE_SUFFIX);
    if (measuringRange != -1) {
      measuring_range->publish_state(measuringRange);
    }

    // OBIS line 16-17 omitted, continue with line 18
    for (int i = 0; i <= 2; i++) {
      line = readLine();
    }

    float flowHours = extractValue(line, FLOW_HOURS_PREFIX, FLOW_HOURS_SUFFIX);
    if (flowHours != -1) {
      flow_hours->publish_state(flowHours);
    }

    // OBIS line 19 to 23 omitted
    while (line.indexOf('!') == -1 && lineNum <= 25) {
      line = readLine();
    }

    lineNum = 1;
    Serial.flush();
    Serial.end();
  }

  float extractValue(const String& line, const String& prefix, const String& suffix) {
    const size_t prefixIndex = line.indexOf(prefix);
    if (prefixIndex == std::string::npos) {
      return -1;
    }
    const size_t startIndex = prefixIndex + prefix.length();
    const size_t endIndex = line.indexOf(suffix, startIndex);
    if (endIndex == std::string::npos) {
      return -1;
    }
    String valueString = line.substring(startIndex, endIndex);
    return valueString.toFloat();
  }

  String readLine() {
    String line = Serial.readStringUntil('\n');
    const size_t lineLength = line.length();
    if (lineLength > 0 && line[lineLength - 1] == '\r') {
      line.remove(lineLength - 1);
    }
    ESP_LOGD("response", "Line %d: %s", lineNum, line.c_str());
    lineNum++;
    return line;
  }
};

