import idapro, idaapi
print("idapro attrs:", [a for a in dir(idapro) if not a.startswith("_")])
print("idaapi get_imagebase:", hasattr(idaapi,"get_imagebase"))
print("idc get_inf_attr:", hasattr(__import__("idc"),"get_inf_attr"))
import idautils
try:
    print("imagebase via idaapi:", hex(idaapi.get_imagebase()))
except Exception as e:
    print("ERR",e)
