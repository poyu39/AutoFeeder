const char page_part1[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
    "<meta charset=\"utf-8\"/>"
"</head>"
"<body>"
    "<form method=\"get\" action=\"set_schedule\" style=\"transform: scale(3); margin-top: 100px;\">"
        "<div style=\"text-align: center;\">"
            "<label for=\"time\">設定排程:</label>"
            "<input type=\"time\" id=\"time\" name=\"time\">"
            "<input type=\"submit\" value=\"確定\">"
        "</div>"
    "</form>"
    "<div style=\"text-align: center; margin-top: 50px; font-size: 50px;\">"
        "<ul>"
;

const char page_part2[] =
            "<li style=\"list-style-type: None\">"
                "<button onclick=\"location.href='delete_schedule?index=${index}'\" style=\"width: 30vh; height: 3vh; font-size: 2vh;\">${time}</button>"
            "</li>"
;

const char page_part3[] =
        "</ul>"
    "</div>"
"</body>"
"</html>"
;