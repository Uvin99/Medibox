const min2 = flow.get("min2");
const max2 = flow.get("max2");
const humi = flow.get("humi");

let result;

if (humi < min2) {
    result = "Warning! Humidity is LOW";
} else if (humi > max2) {
    result = "Warning! Humidity is HIGH";
}
else {
    result = "Humidity is Okay";

}


return {
    out: result
};