let date = new Date();
let hour = date.getHours();
let minute = date.getMinutes();
let schedule = flow.get("schedule");
let days = flow.get("days");
let alarmTriggered = 0;

//Alarm 1
// 19800000 is the shift with UTC (5.30) which need to be reduced.

let A1_en = flow.get("A1_en");
let A1_date = new Date(flow.get("A1_Time") - 19800000);


//Alarm 2
let A2_en = flow.get("A2_en");
let A2_date = new Date(flow.get("A2_Time") - 19800000);

//Alarm 3
let A3_en = flow.get("A3_en");
let A3_date = new Date(flow.get("A3_Time") - 19800000);


const A_hours = [A1_date.getHours(), A2_date.getHours(), A3_date.getHours()];
const A_minutes = [A1_date.getMinutes(), A2_date.getMinutes(), A3_date.getMinutes()];
const A_en = [A1_en, A2_en, A3_en];

if (schedule && days > 0) {
    for (let i = 0; i < 3; i++) {
        if (A_en[i] && A_hours[i] == hour && A_minutes[i] == minute) {

            msg.payload = i;
            alarmTriggered = 1;

            //return msg;
        }

    }
    msg.trig = alarmTriggered
    return msg;
}









