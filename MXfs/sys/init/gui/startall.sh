printf:"Starting video driver"
driver:"/sys/svs/vesadriver.sv"
wait:"vbedriver"

printf:"Starting zipNET windowserver"
exec:"/bin/zipNET"
wait:"zipNET/registration"

printf:"Starting CandyGui"
exec:"/bin/CandyGui"