let days = flow.get("days");
days = Math.max(days - 1, 0);
msg.payload = days;
return msg;