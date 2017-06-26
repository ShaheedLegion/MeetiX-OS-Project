printf:"Starting time driver"
driver:"/sys/eva/server/timedriver.sv"

printf:"Starting ps2 driver"
driver:"/sys/eva/server/ps2driver.sv"

printf:"Starting video driver"
driver:"/sys/eva/server/vbedriver.sv"
wait:"vbedriver"

printf:"Starting zipNET windowserver"
exec:"/app/zipNET/bin/zipNET"
wait:"zipNET::registration"

printf:"Starting MXLogin"
exec:"/app/MXLogin/bin/MXLogin"
