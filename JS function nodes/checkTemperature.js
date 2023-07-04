const min1 = flow.get("min1");
const max1 = flow.get("max1");
const temp = flow.get("temp");

let result;

if (temp < min1) {
    result = "Warning! Temperature is LOW";
} else if (temp > max1) {
    result = "Warning! Temperature is HIGH";
}
else {
    result = "Temperature is Okay";

}


return {
    out: result
};