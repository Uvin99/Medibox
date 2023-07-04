let date = new Date();
let hour = date.getHours();
let minute = date.getMinutes();

let day = date.getDate();
let month = date.getMonth() + 1;
let year = date.getFullYear();

function formatTime(hour, minute) {

    let hourStr = hour.toString();
    let minuteStr = minute.toString();


    if (hour < 10) {
        hourStr = "0" + hourStr;
    }
    if (minute < 10) {
        minuteStr = "0" + minuteStr;
    }


    let timeStr = hourStr + ":" + minuteStr;

    return timeStr;
}


function formatDate(day, month, year) {
    // Convert day, month, and year to strings
    let dayStr = day.toString();
    let monthStr = month.toString();
    let yearStr = year.toString();

    // Pad the strings with leading zeros if necessary
    if (day < 10) {
        dayStr = "0" + dayStr;
    }
    if (month < 10) {
        monthStr = "0" + monthStr;
    }

    // Combine the day, month, and year into a string
    let dateStr = dayStr + "/" + monthStr + "/" + yearStr;

    return dateStr;
}


// Alarm 1
// 19800000 is the shift with UTC (5.30) which need to be reduced.
let A1_date = new Date(flow.get("A1_Time") - 19800000);

//Alarm 2
let A2_date = new Date(flow.get("A2_Time") - 19800000);

//Alarm 3
let A3_date = new Date(flow.get("A3_Time") - 19800000);


const A_hours = [A1_date.getHours(), A2_date.getHours(), A3_date.getHours()];
const A_minutes = [A1_date.getMinutes(), A2_date.getMinutes(), A3_date.getMinutes()];



let currentInfo = formatDate(day, month, year) + " "
    + formatTime(hour, minute) + " "
    + formatTime(A_hours[0], A_minutes[0]) + " "
    + formatTime(A_hours[1], A_minutes[1]) + " "
    + formatTime(A_hours[2], A_minutes[2]);
msg.payload = currentInfo;
return msg;











