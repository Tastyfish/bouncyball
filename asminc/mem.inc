.import _memset, _bzero

.importzp sp, ptr1, ptr2, tmp1
addr1 := ptr1
addr2 := ptr2
sreg := tmp1

.import pusha, pushax, addysp
ppha := pusha
pphax := pushax
