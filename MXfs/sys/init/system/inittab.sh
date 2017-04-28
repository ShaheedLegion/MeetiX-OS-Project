printf:"Starting time driver"
driver:"/sys/svs/timedriver.sv"

printf:"Starting ps2 driver"
driver:"/sys/svs/ps2driver.sv"

printf:"Starting video driver"
driver:"/sys/svs/vesadriver.sv"
wait:"vbedriver"

printf:"Starting zipNET windowserver"
exec:"/bin/zipNET"
wait:"zipNET/registration"

printf:"Starting MXLogin"
exec:"/bin/MXLogin"