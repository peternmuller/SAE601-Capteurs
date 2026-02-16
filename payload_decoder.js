function decodeUplink(input) {
  var decoded = {};
  
  decoded.temperature = (input.bytes[0] / 4 - 20)
  decoded.humidite = (input.bytes[1] / 2)
  
  return {
    data: decoded,
    warnings: [],
    errors: []
  };
}
