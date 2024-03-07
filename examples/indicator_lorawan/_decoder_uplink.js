// Decode uplink function.
//
// Input is an object with the following fields:
// - bytes = Byte array containing the uplink payload, e.g. [255, 230, 255, 0]
// - fPort = Uplink fPort.
// - variables = Object containing the configured device variables.
//
// Output must be an object with the following fields:
// - data = Object representing the decoded payload.
// - warnings = Warnings during decoding (optional).
const sensorTypes = {
  0: { type: "SCD41_CO2", num: 0 },
  1: { type: "SGP40_TVOC", num: 0 },
  2: { type: "SHT41_TEMP", num: 2 },
  3: { type: "SHT41_HUMIDITY", num: 2 },
  4: { type: "SEN5X_Pm2p5", num: 0 },
  5: { type: "SEN5X_Humidity", num: 2 },
  6: { type: "SEN5X_Temperature", num: 2 },
  7: { type: "SEN5X_vocIndex", num: 0 },
  8: { type: "SEN5X_noxIndex", num: 0 },
  9: { type: "SFA3X_HCHO", num: 0 },
  10: { type: "SFA3X_HUMIDITY", num: 2 },
  11: { type: "SFA3X_TEMP", num: 2 },
};
function decodeUplink(input) {
  const decoded = { data: [] };
  const inputBytes = input.bytes;
  let index = 0; // Initialize index to start at the beginning of inputBytes

  // Assuming the last byte is data_len
  const data_len = inputBytes[inputBytes.length - 1];
  
  // Check for length validity (subtract 1 for data_len byte itself)
  if ((inputBytes.length - 1) % 5 !== 0) {
    decoded.data.push({
      type: "error",
      value: -1,
    });
    return { data: decoded };
  }

  while (index < (data_len * 5)) { // 5 bytes per sensor data (1 byte ID + 4 bytes value)
    const sensorId = inputBytes[index++];
    const arrayBuffer = new ArrayBuffer(4);
    const dataView = new DataView(arrayBuffer);

    for (let i = 0; i < 4; i++) {
      dataView.setUint8(i, inputBytes[index++]);
    }

    const floatNumber = dataView.getFloat32(0, true);
    let type = "unknown";
    let num = 0;

    if (sensorTypes[sensorId] !== undefined) {
      type = sensorTypes[sensorId].type;
      num = sensorTypes[sensorId].num;
    }

    decoded.data.push({
      type: type,
      value: floatNumber.toFixed(num),
    });
  }

  return { data: decoded };
}
